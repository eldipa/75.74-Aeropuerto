#ifndef CONEXIONES_AEROPUERTO_H_
#define CONEXIONES_AEROPUERTO_H_

#include "cintas.h"
#include "api_carga.h"
#include "api_admincontenedores.h"

const int cantidad_cintas_checkin = 1;
const int cantidad_cintas_scanner = 1;
const int cantidad_cintas_centrales = 1;
const int cantidad_cintas_contenedor = 2;
const int cantidad_robots_carga = 2;

class TorreDeControl {
public:
	TorreDeControl(char *path_lock_torre_de_control) :
			control(std::vector<short unsigned int>(CANT_MUTEX_CENTRAL, 1),
					path_lock_torre_de_control, MTX_CENTRAL), checkin(path_lock_torre_de_control,
					Q_CHECKINS_HABILITADOS, 0644, true), trasbordo(path_lock_torre_de_control,
					Q_TRASBORDO_LISTO, 0644, true) {
	}
	;

	virtual ~TorreDeControl() {
	}
	;

private:
	SemaphoreSet control;
	MessageQueue checkin;
	MessageQueue trasbordo;
};

/*
 * Clase para crear fácilmente todo lo que se necesite en el aeropuerto
 */
class ConexionesAeropuerto {
public:
	ConexionesAeropuerto(const char *path_to_locks) {
		char path_lock[256];

		Log::info("Creando ipcs para Torre de control...%s%s", path_to_locks,
				PATH_TORRE_DE_CONTROL);
		snprintf(path_lock, 256, "%s%s", path_to_locks, PATH_TORRE_DE_CONTROL);
		torre_de_control = new TorreDeControl(path_lock);

		Log::info("Creando ipcs admin de contenedores...%s%s", path_to_locks,
				PATH_ADMIN_CONTENEDORES);
		snprintf(path_lock, 256, "%s%s", path_to_locks, PATH_ADMIN_CONTENEDORES);
		admin_contenedores = new ApiAdminContenedores(path_lock, CANT_CONTENEDORES_INICIAL);

		Log::info("Creando cintas...");

		snprintf(path_lock, 256, "%s%s", path_to_locks, PATH_CINTA_CHECKIN);
		for (int i = 0; i < cantidad_cintas_checkin; i++) {
			cintas_checkin[i] = new CintaCheckin(path_lock, i, true);
		}

		snprintf(path_lock, 256, "%s%s", path_to_locks, PATH_CINTA_SCANNER);
		for (int i = 0; i < cantidad_cintas_scanner; i++) {
			cintas_scanner[i] = new CintaScanner(path_lock, i, true);
		}

		snprintf(path_lock, 256, "%s%s", path_to_locks, PATH_CINTA_CENTRAL);
		for (int i = 0; i < cantidad_cintas_centrales; i++) {
			//cintas_central[i] = new CintaCentral(true,path_lock,i);
			cintas_central[i] = new CintaCentral(path_lock,
					CINTA_PRINCIPAL_CANTIDAD_MAXIMA_PRODUCTORES,
					CINTA_PRINCIPAL_CANTIDAD_DESPACHANTES, CINTA_PRINCIPAL_TAMANIO_CINTA_CENTRAL);
		}

		snprintf(path_lock, 256, "%s%s", path_to_locks, PATH_CINTA_CONTENEDOR);
		for (int i = 0; i < cantidad_cintas_contenedor; i++) {
			cintas_contenedor[i] = new CintaContenedor(path_lock, i, true);
		}

		Log::info("Creando colas...");
		snprintf(path_lock, 256, "%s%s", path_to_locks, PATH_COLA_ROBOTS_ZONA_TRACTORES);
		cola_robot_zona_tractores = new MessageQueue(path_lock, 0, 0664, true);

		snprintf(path_lock, 256, "%s%s", path_to_locks, PATH_COLA_TRACTORES_AVIONES);
		cola_tractores_avion = new MessageQueue(path_lock, 0, 0664, true);

		snprintf(path_lock, 256, "%s%s", path_to_locks, PATH_MUTEX_ROBOT_CARGA_DESPACHANTE);
		std::vector<unsigned short> valores;
		valores.push_back(1);
		for (int i = 0; i < cantidad_robots_carga; i++) {
			mutexs_carga[i] = new SemaphoreSet(valores, path_lock, i, 0664);
		}

		snprintf(path_lock, 256, "%s%s", path_to_locks, PATH_MEM_ROBOT_CARGA_DESPACHANTE);
		for (int i = 0; i < cantidad_robots_carga; i++) {
			memories_carga[i] = new SharedMemory(path_lock, i, 3 * sizeof(int), 0664, true, false);
		}

	}
	;

	virtual ~ConexionesAeropuerto() {

		for (int i = 0; i < cantidad_cintas_checkin; i++) {
			delete cintas_checkin[i];
		}
		for (int i = 0; i < cantidad_cintas_scanner; i++) {
			delete cintas_scanner[i];
		}
		for (int i = 0; i < cantidad_cintas_centrales; i++) {
			delete cintas_central[i];
		}
		for (int i = 0; i < cantidad_cintas_contenedor; i++) {
			delete cintas_contenedor[i];
		}
		for (int i = 0; i < cantidad_robots_carga; i++) {
			delete mutexs_carga[i];
		}
		for (int i = 0; i < cantidad_robots_carga; i++) {
			delete memories_carga[i];
		}

		delete torre_de_control;
		delete admin_contenedores;

		delete cola_robot_zona_tractores;
		delete cola_tractores_avion;
	}

private:
	TorreDeControl * torre_de_control;
	CintaCheckin * cintas_checkin[1];
	CintaScanner * cintas_scanner[1];
	CintaCentral * cintas_central[1];
	CintaContenedor * cintas_contenedor[2];
	ApiAdminContenedores * admin_contenedores;
	MessageQueue * cola_robot_zona_tractores;
	MessageQueue * cola_tractores_avion;
	SemaphoreSet * mutexs_carga[2];
	SharedMemory * memories_carga[2];
};

#endif
