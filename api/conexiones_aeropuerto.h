#ifndef CONEXIONES_AEROPUERTO_H_
#define CONEXIONES_AEROPUERTO_H_

#include "cintas.h"
#include "api_carga.h"
#include "api_torre_de_control.h"
#include "api_despachante.h"
#include "api_checkin.h"
#include "api_control_equipajes.h"
#include "api_trasbordo.h"
#include <sys/stat.h>
#include <cerrno>
#include <cstdio>
#include <fcntl.h>

#include "yasper.h"
#include <vector>
#include "sharedobject.h"
#include <string>


#include "yasper.h"
#include "iqueue_manager.h"
#include "ipc_queue_manager.h"

#include "message_broker.h"

const int cantidad_cintas_checkin = 1;
const int cantidad_cintas_scanner = 1;
const int cantidad_cintas_centrales = 1;
const int cantidad_cintas_contenedor = 4;
const int cantidad_robots_carga = 4;
const int cantidad_puestos_checkin = 3;
const int cantidad_robots_sospechosos = 1;

class TorreDeControl {
public:
	TorreDeControl(const char *directorio_de_trabajo, int cant_contenedores, int zona_desde,
                  int zona_hasta, int puesto_checkin_desde, int puesto_checkin_hasta, 
                  yasper::ptr<IQueueManager> queue_manager) :

      control(std::vector<short unsigned int>(CANT_MUTEX_CENTRAL, 1),
              std::string(directorio_de_trabajo).append(PATH_TORRE_DE_CONTROL).c_str(),MTX_CENTRAL), 

      checkin( queue_manager->get_queue(PATH_TORRE_DE_CONTROL, Q_CHECKINS_HABILITADOS, true) ),
      trasbordo(queue_manager->get_queue(PATH_TORRE_DE_CONTROL, Q_TRASBORDO_LISTO, true) ),
      queue_zonas(queue_manager->get_queue(PATH_TORRE_DE_CONTROL, Q_ZONAS, true) ),
      queue_puestos_checkin(queue_manager->get_queue(PATH_TORRE_DE_CONTROL, Q_PUESTOS_CHECKIN, true)),
      queue_contenedores(queue_manager->get_queue(PATH_TORRE_DE_CONTROL, Q_CONTENEDORES, true)) {

		ApiTorreDeControl api_torre(directorio_de_trabajo);
		for (int i = 0; i < cant_contenedores; i++) {
			api_torre.liberar_contenedor();
		}
      
		for (int i = zona_desde; i <= zona_hasta; i++) {
			api_torre.liberar_zona(i);
		}
      
		for (int i = puesto_checkin_desde; i <= puesto_checkin_hasta; i++) {
			api_torre.liberar_puesto_checkin(i);
		}

	}
	;

	virtual ~TorreDeControl() {
	}
	;

private:
	SemaphoreSet control;

   yasper::ptr<IMessageQueue> checkin;
   yasper::ptr<IMessageQueue> trasbordo;
   yasper::ptr<IMessageQueue> queue_zonas;
   yasper::ptr<IMessageQueue> queue_puestos_checkin;
   yasper::ptr<IMessageQueue> queue_contenedores;

};

class PuestoCheckin {
public:
	PuestoCheckin(char* path_puesto_checkin, int id_puesto_checkin, int id_cinta_checkin, yasper::ptr<IQueueManager> queue_manager) :
			sem_checkin_realizado(std::vector<unsigned short>(1, 1), path_puesto_checkin,id_puesto_checkin * cant_ipcs), 
         queue_pasajeros(queue_manager->get_queue(PATH_PUESTO_CHECKIN, id_puesto_checkin * cant_ipcs + 1, true)),
         vuelo_actual(tVueloEnCheckin(id_cinta_checkin), path_puesto_checkin,id_puesto_checkin * cant_ipcs + 2) {

		Log::info("creando puesto checkin id_cinta=%d", id_cinta_checkin);
	}
	virtual ~PuestoCheckin() {
	}
private:
	static const int cant_ipcs = 3;
	SemaphoreSet sem_checkin_realizado;

   yasper::ptr<IMessageQueue> queue_pasajeros;
	SharedObject<tVueloEnCheckin> vuelo_actual;
};

class ControladorPuestoCheckin {
public:
	ControladorPuestoCheckin(yasper::ptr<IQueueManager> queue_manager) :
      queue_checkin(queue_manager->get_queue(PATH_COLA_CONTROL_CHECKIN, 0, true)) {
	}
private:
   yasper::ptr<IMessageQueue> queue_checkin;
};

class RobotsDespacho {
public:
	RobotsDespacho(int id_robot, char* path_robots_despacho) :
			sem_set(std::vector<unsigned short>(1, 1), path_robots_despacho,
					ApiDespachante::cant_ipcs * id_robot), asignaciones(ZonasAsignadas(),
					path_robots_despacho, id_robot * ApiDespachante::cant_ipcs + 1) {
	}
private:
	SemaphoreSet sem_set;
	SharedObject<ZonasAsignadas> asignaciones;
};

/*
 * Clase para crear fácilmente todo lo que se necesite en el aeropuerto
 */
class ConexionesAeropuerto {
public:
	ConexionesAeropuerto(const char *path_to_locks, yasper::ptr<IQueueManager> queue_manager) : queue_manager(queue_manager) {
		char path_lock[256];

		crear_archivos_lck(path_to_locks);

		Log::info("Creando ipcs para Message Broker en share memory...%s%s", path_to_locks,PATH_MESSAGE_BROKER);
      shm_broker = new MessageBroker(path_to_locks, true);

		Log::info("Creando ipcs para Controlador de puestos de checkin...%s%s", path_to_locks,
				PATH_COLA_CONTROL_CHECKIN);
		snprintf(path_lock, 256, "%s%s", path_to_locks, PATH_COLA_CONTROL_CHECKIN);
		controlador_puesto_checkin = new ControladorPuestoCheckin(queue_manager);

		Log::info("Creando ipcs para Puesto de checkin...%s%s", path_to_locks, PATH_PUESTO_CHECKIN);
		snprintf(path_lock, 256, "%s%s", path_to_locks, PATH_PUESTO_CHECKIN);
		for (int i = 0; i < cantidad_puestos_checkin; i++) {
			puesto_checkin.push_back(new PuestoCheckin(path_lock, i + 1, 1, queue_manager));
		}

		Log::info("Creando ipcs para Robots de despacho...%s%s", path_to_locks,
				PATH_ROBOT_DESPACHO);
		snprintf(path_lock, 256, "%s%s", path_to_locks, PATH_ROBOT_DESPACHO);
		for (int i = 0; i < cantidad_robots_carga; i++) {
			robots_despacho.push_back(new RobotsDespacho(i + 1, path_lock));
		}

		Log::info("Creando ipcs para Torre de control...%s%s", path_to_locks,
				PATH_TORRE_DE_CONTROL);
		//snprintf(path_lock, 256, "%s%s", path_to_locks, PATH_TORRE_DE_CONTROL);
		torre_de_control = new TorreDeControl(path_to_locks, 10, 1, cantidad_robots_carga, 1,
                                            cantidad_puestos_checkin, queue_manager);

		Log::info("Creando cintas...");

		snprintf(path_lock, 256, "%s%s", path_to_locks, PATH_CINTA_CHECKIN);
		for (int i = 0; i < cantidad_cintas_checkin; i++) {
			cintas_checkin.push_back(
					new CintaCheckin(path_lock, i + 1, CAPACIDAD_CINTA_CHECKIN,
							CANTIDAD_MAX_PRODUCTORES_CINTA_CHECKIN,
							CANTIDAD_MAX_CONSUMIDORES_CINTA_CHECKIN));
		}

		snprintf(path_lock, 256, "%s%s", path_to_locks, PATH_CINTA_SCANNER);
		cintas_scanner = new CintaScanner<Equipaje>(path_lock, 1, CAPACIDAD_CINTA_SCANNER, true);

		snprintf(path_lock, 256, "%s%s", path_to_locks, PATH_CINTA_CENTRAL);

		for (int i = 0; i < cantidad_cintas_centrales; i++) {

			cintas_central.push_back(
					new CintaCentral(path_lock, CAPACIDAD_CINTA_CENTRAL,
							CANTIDAD_MAX_PRODUCTORES_CINTA_CENTRAL,
							CANTIDAD_MAX_CONSUMIDORES_CINTA_CENTRAL));
		}

		snprintf(path_lock, 256, "%s%s", path_to_locks, PATH_CINTA_CONTENEDOR);
		for (int i = 0; i < cantidad_cintas_contenedor; i++) {
			cintas_contenedor.push_back(
					new CintaContenedor(path_lock, i + 1, CAPACIDAD_CINTA_CONTENEDOR,
							CANTIDAD_MAX_PRODUCTORES_CINTA_CONTENEDOR,
							CANTIDAD_MAX_CONSUMIDORES_CINTA_CONTENEDOR));
		}

		// snprintf(path_lock, 256, "%s%s", path_to_locks, PATH_COLA_CONTROL_CARGA_CHECKIN);
		for (int i = 0; i < cantidad_robots_carga; i++) {
			// cola_control_carga_checkin.push_back(new MessageQueue(path_lock, i + 1, 0664, true));
         cola_control_carga_checkin.push_back( queue_manager->get_queue(PATH_COLA_CONTROL_CARGA_CHECKIN, i+1, true) );
		}

		Log::info("Creando colas...");
		// snprintf(path_lock, 256, "%s%s", path_to_locks, PATH_COLA_ROBOTS_ZONA_TRACTORES);
		// cola_robot_zona_tractores = new MessageQueue(path_lock, 0, 0664, true);
      cola_robot_zona_tractores = queue_manager->get_queue(PATH_COLA_ROBOTS_ZONA_TRACTORES, 0, true);

		// snprintf(path_lock, 256, "%s%s", path_to_locks, PATH_COLA_TRACTORES_AVIONES);
		// cola_tractores_avion = new MessageQueue(path_lock, 0, 0664, true);
      cola_tractores_avion = queue_manager->get_queue(PATH_COLA_TRACTORES_AVIONES, 0, true);

		for (int i = 0; i < cantidad_robots_sospechosos; i++) {
			control_equipajes.push_back(new ApiControlEquipajes(path_to_locks, 3, 3, true));
		}

		// INTERCARGO
		// snprintf(path_lock, 256, "%s%s", path_to_locks, PATH_COLA_ESCUCHA_ZONA_ASIGNADA);
		// cola_escucha_vuelos_entrantes = new MessageQueue(path_lock, 0, 0664, true);
      cola_escucha_vuelos_entrantes = queue_manager->get_queue(PATH_COLA_ESCUCHA_ZONA_ASIGNADA, 0, true);

		trasbordo = new ApiTrasbordo(path_to_locks, true);

	};

	virtual ~ConexionesAeropuerto() {
	}

private:
   yasper::ptr<IQueueManager> queue_manager;

	std::vector<yasper::ptr<RobotsDespacho> > robots_despacho;
	yasper::ptr<ControladorPuestoCheckin> controlador_puesto_checkin;
	std::vector<yasper::ptr<PuestoCheckin> > puesto_checkin;
	yasper::ptr<TorreDeControl> torre_de_control;
	std::vector<yasper::ptr<CintaCheckin> > cintas_checkin;
	yasper::ptr<CintaScanner<Equipaje> > cintas_scanner;
	std::vector<yasper::ptr<CintaCentral> > cintas_central;
	std::vector<yasper::ptr<CintaContenedor> > cintas_contenedor;
	yasper::ptr<IMessageQueue> cola_robot_zona_tractores;
	yasper::ptr<IMessageQueue> cola_tractores_avion;
	std::vector<yasper::ptr<IMessageQueue> > cola_control_carga_checkin;
	std::vector<yasper::ptr<ApiControlEquipajes> > control_equipajes;
	yasper::ptr<IMessageQueue> cola_escucha_vuelos_entrantes;
	yasper::ptr<ApiTrasbordo> trasbordo;

   yasper::ptr<MessageBroker> shm_broker;

	void crear_archivos_lck(const char *path_to_locks) {

		struct stat buf;
		int result;

		result = stat(path_to_locks, &buf);

		if (result != 0) {
			/* Directory does not exist. EEXIST for race condition */
			if (mkdir(path_to_locks, 0770) != 0 && errno != EEXIST) {
				//THROW OSERROR
			}
		} else if (!S_ISDIR(buf.st_mode)) {
			errno = ENOTDIR;
			//THROW OSERROR
		}

		crear_archivo_lck(path_to_locks, PATH_CINTA_CHECKIN);
		crear_archivo_lck(path_to_locks, PATH_CINTA_SCANNER);
		crear_archivo_lck(path_to_locks, PATH_CINTA_CENTRAL);
		crear_archivo_lck(path_to_locks, PATH_CINTA_CONTENEDOR);
		crear_archivo_lck(path_to_locks, PATH_TORRE_DE_CONTROL);
		crear_archivo_lck(path_to_locks, PATH_CONTROLADOR_DE_CARGA);
		crear_archivo_lck(path_to_locks, PATH_ADMIN_CONTENEDORES);
		crear_archivo_lck(path_to_locks, PATH_COLA_TRACTORES_AVIONES);
		crear_archivo_lck(path_to_locks, PATH_COLA_AVIONES_ROBOTS_ZONA);
		crear_archivo_lck(path_to_locks, PATH_COLA_ROBOTS_ZONA_TRACTORES);
		crear_archivo_lck(path_to_locks, PATH_PUESTO_CHECKIN);
		crear_archivo_lck(path_to_locks, PATH_COLA_CONTROL_CARGA_CHECKIN);
		crear_archivo_lck(path_to_locks, PATH_COLA_CONTROL_CHECKIN);
		crear_archivo_lck(path_to_locks, PATH_ROBOT_DESPACHO);
		crear_archivo_lck(path_to_locks, PATH_CONTROL_SOSPECHOSOS);
		crear_archivo_lck(path_to_locks, PATH_IPC_ROBOTS_INTERCARGO);
		crear_archivo_lck(path_to_locks, PATH_COLA_ESCUCHA_ZONA_ASIGNADA);
		crear_archivo_lck(path_to_locks, PATH_COLA_ROBOTS_INTERCARGO);
		crear_archivo_lck(path_to_locks, PATH_MESSAGE_BROKER);
		crear_archivo_lck(path_to_locks, PATH_MESSAGE_BROKER_QUEUES);
	}

	void crear_archivo_lck(const char *path_to_locks, const char * nombre_archivo) {
		char path[300];
		int file;
		struct stat buf;

		strcpy(path, path_to_locks);
		strcat(path, nombre_archivo);

		if (stat(path, &buf) != 0) {
			file = open(path, O_CREAT | 0664);
			if (file != -1) {
				close(file);
			} else {
				//THROW OSERROR
			}
		}
	}
};

#endif
