#ifndef API_CARGA_H_
#define API_CARGA_H_

#include "equipaje.h"
#include "contenedor.h"
#include "api_constants.h"
#include "mensajes.h"
#include "log.h"
#include "semaphoreset.h"
#include "sharedmemory.h"
#include "mutex.h"
#include "cintas.h"
#include <vector>
#include <string>
#include "api_common.h"

#include "yasper.h"
#include "iqueue_manager.h"
#include "imessagequeue.h"
#include "api_configuracion.h"
#include <sys/stat.h>
#include "dir.h"

class ApiCarga {
private:
	//std::string path;
	int id_robot_carga;
	std::vector<Contenedor> contenedores;

	yasper::ptr<IQueueManager> queue_manager;
	yasper::ptr<IMessageQueue> cola_tractores;
	yasper::ptr<IMessageQueue> cola_aviso_carga;

	yasper::ptr<CintaContenedor> cinta_contenedor;

	int * cerro_el_checkin;
	int * cant_equipaje_total;
	//int * cant_equipaje_restante;

public:

	/*
	 * Api que maneja la sincronizacion y el pasaje de contenedores entre el robot_carga y el controlador_de_carga.
	 * El primer constructor crea los ipcs.Se llama solo una vez por cada controlador_de_carga.
	 * recibe un path_carga y el id_robot_carga.Va a existir una ApiCarga por cada robot_carga.
	 **/
	ApiCarga(const char * app_name, const char * directorio_de_trabajo, const char* config_file, int id_robot_carga,
		int num_cinta, bool same_dir)
		: id_robot_carga(id_robot_carga),
			queue_manager(ApiConfiguracion::get_queue_manager(directorio_de_trabajo, config_file)),
			cola_tractores(queue_manager->get_queue(PATH_COLA_ROBOTS_ZONA_TRACTORES, 0)),
			cola_aviso_carga(queue_manager->get_queue(PATH_COLA_CONTROL_CARGA_CHECKIN, id_robot_carga))

	{
		if (same_dir) {
			cinta_contenedor = new CintaContenedor(app_name, directorio_de_trabajo, num_cinta, true);
		} else {
			mkdir(std::string(directorio_de_trabajo).append("/other_dir").c_str(), 0770);
			cp(std::string(directorio_de_trabajo).append("/other_dir").append(PATH_LOCAL_BROKER_LIST_FILE).c_str(),
				std::string(directorio_de_trabajo).append(PATH_LOCAL_BROKER_LIST_FILE).c_str());
			cinta_contenedor = new CintaContenedor(app_name,
				std::string(directorio_de_trabajo).append("/other_dir").c_str(), num_cinta, true);
		}
	}

	ApiCarga(const char * directorio_de_trabajo, const char* config_file, int id_robot_carga)
		: id_robot_carga(id_robot_carga),
			queue_manager(ApiConfiguracion::get_queue_manager(directorio_de_trabajo, config_file)),
			cola_tractores(queue_manager->get_queue(PATH_COLA_ROBOTS_ZONA_TRACTORES, 0)),
			cola_aviso_carga(queue_manager->get_queue(PATH_COLA_CONTROL_CARGA_CHECKIN, id_robot_carga))
	{
		//cinta_contenedor = new CintaContenedor(app_name, directorio_de_trabajo, num_cinta, false);
	}

	~ApiCarga() {
	}

	bool checkin_cerrado() {
		return cinta_contenedor->checkin_ya_cerro();
	}

	Equipaje sacar_equipaje() {
		return cinta_contenedor->sacar_equipaje();
	}

	int obtener_cantidad_equipaje_total() {
		return cinta_contenedor->cantidad_valijas_totales();
	}

	/*
	 * Guarda el contenedor con equipajes recibidos por el robot_carga.
	 * Guarda los contenedores en una sharedmemory
	 **/
	void agregar_contenedor_cargado(Contenedor& c) {
		contenedores.push_back(c);
	}

	/* Api que llama el robot de zona para entregarle los contenedores
	 * a los tractores
	 */
	void enviar_contenedores_a_avion(int numero_de_vuelo) {
		BloqueContenedores bloque;
		int cantidad_total_contenedores;
		int cant_bloques;
		int i,j;

		cantidad_total_contenedores = contenedores.size();
		bloque.mtype = ID_ESCUCHA_TRACTOR;
		bloque.cantidad_total_contenedores = cantidad_total_contenedores;
		bloque.vuelo_destino = numero_de_vuelo;

		cant_bloques = cantidad_total_contenedores / MAX_CONTENEDORES_POR_TRACTOR;
		if (cantidad_total_contenedores > cant_bloques * MAX_CONTENEDORES_POR_TRACTOR) {
			cant_bloques++;
		}

		for (i = 0; i < cant_bloques ; i++) {
			bloque.contenedores_actual = 0;
			for (j = 0; j < MAX_CONTENEDORES_POR_TRACTOR ; j++) {
				bloque.contenedores [j] = contenedores [i * MAX_CONTENEDORES_POR_TRACTOR + j];
				bloque.contenedores_actual++;
			}
			Log::info("Enviando Bloque %d/%d a tractores tamaño %d #vuelo= %d", i + 1, cant_bloques,
				sizeof(BloqueContenedores), numero_de_vuelo);
			cola_tractores->push((const void *)&bloque, sizeof(BloqueContenedores) - sizeof(long));
		}

		contenedores.clear();

	}

	/*
	 parche: resetea la cinta contenedor cuando cerro el checkin
	 TODO: parche temporal para que el robot_carga no siga cargando equipagjes de otro vuelo
	 en los mismo contenedores.
	 */
	void comenzar_nueva_carga() {
		cinta_contenedor->comenzar_nueva_carga();
	}

	void cargar_equipajes(int equipajes_por_cargar) {
		MENSAJE_CHECKIN_CERRADO mensaje;
		mensaje.mtype = 1;
		mensaje.checkin_cerrado = 1;
		mensaje.cantidad_equipaje_total = equipajes_por_cargar;
		cola_aviso_carga->push(&mensaje, sizeof(MENSAJE_CHECKIN_CERRADO) - sizeof(long));
	}

	int get_equipajes_por_cargar() {
		MENSAJE_CHECKIN_CERRADO mensaje;
		cola_aviso_carga->pull(&mensaje, sizeof(MENSAJE_CHECKIN_CERRADO) - sizeof(long), 0);
		return mensaje.cantidad_equipaje_total;

	}

	void avisar_cierre_de_checkin(int equipajes_por_cargar) {
		cinta_contenedor->avisar_cierre_de_checkin(equipajes_por_cargar);
	}

private:

};

#endif
