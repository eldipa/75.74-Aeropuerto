#include "api_comunicacion_intercargo.h"

#include <string>
using namespace std;
#include "api_constants.h"

#include "api_configuracion.h"

ApiComunicacionIntercargo::ApiComunicacionIntercargo(const char * directorio_de_trabajo, const char* config_file, bool create) :
   queue_manager( ApiConfiguracion::get_queue_manager(directorio_de_trabajo, config_file) ),
   cola_cargadores_equipaje( queue_manager->get_queue(PATH_COLA_ROBOTS_INTERCARGO, 0,true) ) {
   create = create;
}

ApiComunicacionIntercargo::ApiComunicacionIntercargo(const char * directorio_de_trabajo, const char* config_file) :
   queue_manager( ApiConfiguracion::get_queue_manager(directorio_de_trabajo, config_file) ),
   cola_cargadores_equipaje( queue_manager->get_queue(PATH_COLA_ROBOTS_INTERCARGO, 0) ) {
}

ApiComunicacionIntercargo::~ApiComunicacionIntercargo() {
}

void ApiComunicacionIntercargo::informar_vuelo_entrante(int numero_vuelo_entrante,
		int numero_vuelo_destino) {
	mensaje.mtype = numero_vuelo_destino;
	mensaje.vuelo_entrante = numero_vuelo_entrante;
	cola_cargadores_equipaje->push(&mensaje, sizeof(MENSAJE_VUELO_ENTRANTE) - sizeof(long));
}

int ApiComunicacionIntercargo::esperar_vuelo_entrante(int numero_vuelo_destino) {
	mensaje.mtype = 0;
	cola_cargadores_equipaje->pull(&this->mensaje, sizeof(MENSAJE_VUELO_ENTRANTE), numero_vuelo_destino);
	return mensaje.vuelo_entrante;
}
