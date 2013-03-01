#include "api_comunicacion_aeropuerto.h"

#include "api_configuracion.h"
#include "genericerror.h"
#include "mensajes.h"

   // yasper::ptr<IQueueManager> queue_manager;
   // yasper::ptr<IMessageQueue> queue_zona_asignada;
   // // yasper::ptr<IMessageQueue> queue_equipaje_to_aeropuerto;

	// int id_aeropuerto;

ApiComunicacionAeropuerto::ApiComunicacionAeropuerto(const char* directorio_de_trabajo, const char* config_file) :
   queue_manager(ApiConfiguracion::get_intermediate_queue_manager(directorio_de_trabajo, config_file)),
   queue_zona_asignada(queue_manager->get_queue(PATH_COLA_ESCUCHA_ZONA_ASIGNADA, 0)),
   queue_equipaje_to_aeropuerto(queue_manager->get_queue(PATH_COLA_INTERCARGO,0)) {
}
                       
ApiComunicacionAeropuerto::ApiComunicacionAeropuerto(const char* directorio_de_trabajo, const char* config_file, bool create) :
   queue_manager(ApiConfiguracion::get_intermediate_queue_manager(directorio_de_trabajo, config_file)),
   queue_zona_asignada(queue_manager->get_queue(PATH_COLA_ESCUCHA_ZONA_ASIGNADA, 0, true)),
   queue_equipaje_to_aeropuerto(queue_manager->get_queue(PATH_COLA_INTERCARGO,0, true)) {

   create = create;
}

int ApiComunicacionAeropuerto::esperar_zona_asignada(int numero_vuelo) {

   MENSAJE_ZONA_ASIGNADA mensaje;
 	queue_zona_asignada->pull(&mensaje, sizeof(MENSAJE_ZONA_ASIGNADA) - sizeof(long), numero_vuelo);
   //id_productor = mensaje.zona_asignada + MAX_SCANNERS + 1; //?

   Log::debug("ApiComAeropuerto llego mensaje:zona %d asignada a vuelo %d en aeropuerto %d", mensaje.zona_asignada, numero_vuelo, mensaje.id_aeropuerto);
 	return mensaje.id_aeropuerto;
}

void ApiComunicacionAeropuerto::zona_asignada(int num_vuelo, int num_zona, int id_aeropuerto) {
   if(id_aeropuerto==-1)
      throw GenericError("Llamada inválida a zona_asignada sin id_aeropuerto");

   MENSAJE_ZONA_ASIGNADA msg_trasbordo;
   msg_trasbordo.mtype = num_vuelo;
   msg_trasbordo.id_aeropuerto = id_aeropuerto;
   msg_trasbordo.numero_de_vuelo = num_vuelo;
   msg_trasbordo.zona_asignada = num_zona;
   
   queue_zona_asignada->push(&msg_trasbordo, sizeof(MENSAJE_ZONA_ASIGNADA)-sizeof(long));
}

void ApiComunicacionAeropuerto::enviar_equipaje_intercargo(const Equipaje& equipaje, int id_aeropuerto) {
   MENSAJE_EQUIPAJE_INTERCARGO mensaje;
   mensaje.mtype = id_aeropuerto;
   mensaje.equipaje_intercargo = equipaje;
   queue_equipaje_to_aeropuerto->push(&mensaje, sizeof(MENSAJE_EQUIPAJE_INTERCARGO)-sizeof(long));
}

Equipaje ApiComunicacionAeropuerto::get_proximo_equipaje_intercargo(int id_aeropuerto) {
   MENSAJE_EQUIPAJE_INTERCARGO mensaje;
 	queue_equipaje_to_aeropuerto->pull(&mensaje, sizeof(MENSAJE_EQUIPAJE_INTERCARGO) - sizeof(long), id_aeropuerto);
   return mensaje.equipaje_intercargo;
}


ApiComunicacionAeropuerto::~ApiComunicacionAeropuerto() {
}

