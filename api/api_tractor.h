#ifndef APITRACTOR_H_
#define APITRACTOR_H_

#include "api_constants.h"
#include "contenedor.h"
#include <vector>
#include "log.h"
#include <string>

#include "yasper.h"
#include "imessagequeue.h"
#include "iqueue_manager.h"
#include "api_configuracion.h"

class ApiTractor {
private:

	long id;
   yasper::ptr<IQueueManager> queue_manager;
   yasper::ptr<IMessageQueue> cola_avion;
   yasper::ptr<IMessageQueue> cola_robot_zona;

public:
	ApiTractor(const char * directorio_de_trabajo, const char* config_file, int id_tractor) :
      id(id_tractor), 
      queue_manager( ApiConfiguracion::get_queue_manager(directorio_de_trabajo, config_file) ),
      cola_avion( queue_manager->get_queue(PATH_COLA_TRACTORES_AVIONES, 0) ),
      cola_robot_zona( queue_manager->get_queue(PATH_COLA_ROBOTS_ZONA_TRACTORES, 0) ) {

	}

	ApiTractor(const char * directorio_de_trabajo, const char* config_file, int id_tractor, bool create) :
      id(id_tractor), 
      queue_manager( ApiConfiguracion::get_queue_manager(directorio_de_trabajo, config_file) ),
      cola_avion( queue_manager->get_queue(PATH_COLA_TRACTORES_AVIONES, 0, true) ),
      cola_robot_zona( queue_manager->get_queue(PATH_COLA_ROBOTS_ZONA_TRACTORES, 0, true) ) {
      create = create;
	}

	virtual ~ApiTractor() {

	}

 	void obtener_contenedores_a_cargar(BloqueContenedores &contenedores) {
		cola_robot_zona->pull((void *) &contenedores, sizeof(BloqueContenedores) - sizeof(long),
				ID_ESCUCHA_TRACTOR);
		Log::debug("Bloque Recibido: numero_vuelo=%d cantidad_total_contenedores=%d",
				contenedores.vuelo_destino, contenedores.cantidad_total_contenedores);
	}

	void cargar_contenedor_en_avion(const Contenedor & contenedor, int vuelo,
			int cantidad_total_contenedores) {
		ContenedorParaAvion contenedor_para_avion;
		Log::debug("cargar_contenedor_en_avion: numero_vuelo=%d", vuelo);
		contenedor_para_avion.mtype = vuelo;
		contenedor_para_avion.cantidad_total_contenedores = cantidad_total_contenedores;
		contenedor_para_avion.contenedor = contenedor;
		cola_avion->push(&contenedor_para_avion, sizeof(ContenedorParaAvion) - sizeof(long));
	}
};

#endif /* APITRACTOR_H_ */
