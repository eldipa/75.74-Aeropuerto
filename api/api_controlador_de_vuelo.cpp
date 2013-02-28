#include "api_controlador_de_vuelo.h"

#include "api_constants.h"
#include "api_checkin.h"

#include "messagequeue.h"
#include "mensajes.h"

#include "api_configuracion.h"

#include <stdio.h>

ApiControladorDeVuelo::ApiControladorDeVuelo(const char*  directorio_de_trabajo, const char* config_file, int num_vuelo)
   : path_to_locks(directorio_de_trabajo), num_vuelo(num_vuelo),
     queue_manager( ApiConfiguracion::get_queue_manager(directorio_de_trabajo,config_file) ),
     queue_trasbordo( queue_manager->get_queue(PATH_COLA_ESCUCHA_ZONA_ASIGNADA, 0) ),
     queue_checkin( queue_manager->get_queue(PATH_COLA_CONTROL_CHECKIN, 0) ) {
}

void ApiControladorDeVuelo::iniciar_checkin(int num_puesto_checkin, int num_zona) {
   enviar_mensaje_to_checkin(num_puesto_checkin, num_zona, true);

   MENSAJE_ZONA_ASIGNADA msg_trasbordo;
   msg_trasbordo.mtype = this->num_vuelo;
   msg_trasbordo.numero_de_vuelo = this->num_vuelo;
   msg_trasbordo.zona_asignada = num_zona;
   
   //MessageQueue queue_trasbordo(std::string(path_to_locks).append(PATH_COLA_ESCUCHA_ZONA_ASIGNADA).c_str(), 0);
   queue_trasbordo->push(&msg_trasbordo, sizeof(MENSAJE_ZONA_ASIGNADA)-sizeof(long));

}

void ApiControladorDeVuelo::cerrar_checkin(int num_puesto_checkin, int num_zona) {
   enviar_mensaje_to_checkin(num_puesto_checkin, num_zona, false);
}

void ApiControladorDeVuelo::enviar_mensaje_to_checkin(int num_puesto_checkin, int num_zona, bool iniciar_checkin) {
   tMeansajeCheckin msg_checkin;
   msg_checkin.mtype = num_puesto_checkin;
   msg_checkin.iniciar_checkin = iniciar_checkin;
   msg_checkin.num_vuelo = num_vuelo;
   msg_checkin.num_zona = num_zona;

   //   char path_to_control_checkin_lock[128];
   //snprintf(path_to_control_checkin_lock, 128, "%s%s", path_to_locks.c_str(), PATH_COLA_CONTROL_CHECKIN);

   //MessageQueue queue_checkin(path_to_control_checkin_lock, 0);
   queue_checkin->push(&msg_checkin, sizeof(tMeansajeCheckin)-sizeof(long));
}

ApiControladorDeVuelo::~ApiControladorDeVuelo() {
}
