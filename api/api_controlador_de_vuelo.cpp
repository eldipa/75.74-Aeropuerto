#include "api_controlador_de_vuelo.h"

#include "api_constants.h"
#include "api_checkin.h"

#include "messagequeue.h"

#include <stdio.h>

ApiControladorDeVuelo::ApiControladorDeVuelo(const char* path_to_locks, int num_vuelo) 
   : path_to_locks(path_to_locks), num_vuelo(num_vuelo) {
}

void ApiControladorDeVuelo::iniciar_checkin(int num_puesto_checkin) {
   enviar_mensaje_to_checkin(num_puesto_checkin, true);
}
 
void ApiControladorDeVuelo::cerrar_checkin(int num_puesto_checkin) {
   enviar_mensaje_to_checkin(num_puesto_checkin, false);
}

void ApiControladorDeVuelo::enviar_mensaje_to_checkin(int num_puesto_checkin, bool iniciar_checkin) {
   tMeansajeCheckin msg_checkin;
   msg_checkin.mtype = num_puesto_checkin;
   msg_checkin.iniciar_checkin = iniciar_checkin;
   msg_checkin.num_vuelo = num_vuelo;

   char path_to_control_checkin_lock[128];
   snprintf(path_to_control_checkin_lock, 128, "%s%s", path_to_locks.c_str(), PATH_COLA_CONTROL_CHECKIN);
    
   MessageQueue queue_checkin(path_to_control_checkin_lock, 0);
   queue_checkin.push(&msg_checkin, sizeof(tMeansajeCheckin));
}

ApiControladorDeVuelo::~ApiControladorDeVuelo() {
}
