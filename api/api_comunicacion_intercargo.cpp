#include "api_comunicacion_intercargo.h"

#include <string>
using namespace std;
#include "api_constants.h"

#include "ipc_queue_manager.h"

ApiComunicacionIntercargo::ApiComunicacionIntercargo(const char * directorio_de_trabajo) :
   queue_manager( new IpcQueueManager(directorio_de_trabajo) ),
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
