#include "api_trasbordo.h"
#include "api_constants.h"
#include <exception>
#include <stdexcept>
#include <string>
#include <vector>
#include "api_common.h"

#include "api_configuracion.h"

using namespace std;

ApiTrasbordo::ApiTrasbordo(const char* directorio_de_trabajo, const char* config_file, int numero_de_vuelo) :
   queue_manager(ApiConfiguracion::get_queue_manager(directorio_de_trabajo, config_file)),
   cola_cargadores_equipaje(queue_manager->get_queue(PATH_COLA_ROBOTS_INTERCARGO, 0)) {
   // cola_asignaciones( queue_manager->get_queue(PATH_COLA_ESCUCHA_ZONA_ASIGNADA, 0) ) {

	cinta = new CintaCentral(std::string("robot_intercargo").append(intToString(numero_de_vuelo)).c_str(),
		directorio_de_trabajo, -1, -2);

	id_productor = 1 + MAX_SCANNERS + 1;

}

ApiTrasbordo::~ApiTrasbordo() {
}

void ApiTrasbordo::poner_en_cinta_principal(const Equipaje& equipaje) {
	cinta->colocar_elemento(&equipaje, id_productor);
}

int ApiTrasbordo::esperar_vuelo_entrante(int numero_vuelo_destino) {
	mensaje.mtype = 0;
	cola_cargadores_equipaje->pull(&this->mensaje, sizeof(MENSAJE_VUELO_ENTRANTE), numero_vuelo_destino);
	return mensaje.vuelo_entrante;
}

// int ApiTrasbordo::esperar_zona_asignada(int numero_vuelo) {
//    MENSAJE_ZONA_ASIGNADA mensaje;
// 	cola_asignaciones->pull(&mensaje, sizeof(MENSAJE_ZONA_ASIGNADA) - sizeof(long), numero_vuelo);
//    id_productor = mensaje.zona_asignada + MAX_SCANNERS + 1;

//    Log::debug("Refactor: llego mensaje zona %d asignada a vuelo %d", mensaje.zona_asignada, numero_vuelo);
// 	return mensaje.zona_asignada;
// }

