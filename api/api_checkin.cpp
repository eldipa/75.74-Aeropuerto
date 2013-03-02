#include "api_checkin.h"

#include "cintas.h"
#include "api_constants.h"
#include "database.h"
#include "stmt.h"
#include "tupleiter.h"
#include "messagequeue.h"
#include <vector>
#include <unistd.h>
#include <stdlib.h>
#include <cstdio>

#include "equipaje.h"
#include "api_constants.h"
#include <stdexcept>

#include "yasper.h"
#include "api_configuracion.h"

#include "api_common.h"
#include "api_torre_de_control.h"

ApiCheckIn::ApiCheckIn(const char* directorio_de_trabajo, const char* config_file,int id_checkin) :
   path_to_locks(directorio_de_trabajo),
   id_checkin(id_checkin),
   vuelo_actual(std::string(directorio_de_trabajo).append(PATH_PUESTO_CHECKIN).c_str(), id_checkin*cant_ipcs+2),
   cinta_checkin_out(std::string("checkin").append(intToString(id_checkin)).c_str(),std::string(directorio_de_trabajo).c_str(), vuelo_actual->id_cinta_checkin),
   sem_set(std::string(directorio_de_trabajo).append(PATH_PUESTO_CHECKIN).c_str(), id_checkin*cant_ipcs, 1),
   mutex_checkin(sem_set, 0),
   queue_manager( ApiConfiguracion::get_queue_manager(directorio_de_trabajo, config_file ) ),
   queue_pasajeros( queue_manager->get_queue(PATH_PUESTO_CHECKIN, 0) ),
   queue_controlador(queue_manager->get_queue(PATH_COLA_CONTROL_CHECKIN, 0, true)) {
   
}

ApiCheckIn::ApiCheckIn(const char* directorio_de_trabajo, const char* config_file, int id_puesto_checkin, int id_cinta_checkin, bool create ) :
   path_to_locks(directorio_de_trabajo),
   id_checkin(id_puesto_checkin),
   vuelo_actual(tVueloEnCheckin(id_cinta_checkin),std::string(directorio_de_trabajo).append(PATH_PUESTO_CHECKIN).c_str(),id_puesto_checkin * cant_ipcs + 2),
   cinta_checkin_out(std::string(directorio_de_trabajo).append(PATH_CINTA_CHECKIN).c_str(), id_cinta_checkin),
   sem_set(std::vector<unsigned short>(1, 1), std::string(directorio_de_trabajo).append(PATH_PUESTO_CHECKIN).c_str(),id_puesto_checkin * cant_ipcs),
   mutex_checkin(sem_set, 0),
   queue_manager( ApiConfiguracion::get_queue_manager(directorio_de_trabajo, config_file ) ),
   queue_pasajeros( queue_manager->get_queue(PATH_PUESTO_CHECKIN, 0, true) ),
   queue_controlador(queue_manager->get_queue(PATH_COLA_CONTROL_CHECKIN, 0, true)) {

   create = create;
   ApiTorreDeControl api_torre(directorio_de_trabajo, config_file);
   api_torre.liberar_puesto_checkin(id_puesto_checkin);
}


ApiCheckIn::~ApiCheckIn() {
}

void ApiCheckIn::iniciar_checkin( int numero_vuelo ) {
   mutex_checkin.lock();
   if( vuelo_actual->num_vuelo == -1 ) {
      yasper::ptr<IMessageQueue> checkin = queue_manager->get_queue(PATH_TORRE_DE_CONTROL, Q_CHECKINS_HABILITADOS);
      //MessageQueue checkin(std::string(PATH_KEYS).append(PATH_TORRE_DE_CONTROL).c_str(), Q_CHECKINS_HABILITADOS);

      vuelo_actual->num_vuelo = numero_vuelo;
      vuelo_actual->cant_equipajes = 0;
   } else {
      Log::crit("El checkin ya esta abierto en el puesto %d para el vuelo %d", id_checkin, vuelo_actual->num_vuelo);
   }
   mutex_checkin.unlock();
}

int ApiCheckIn::cerrar_checkin() {
   int equipajes_cargados = 0;
	mutex_checkin.lock();

	if (vuelo_actual->num_vuelo == -1) {
		throw std::runtime_error("No había ningún checkin abierto");
	}

   Log::info("Notificando checkin cerrado para vuelo %i", vuelo_actual->num_vuelo);
   //checkin.push(&vuelo_actual, sizeof(vuelo_actual)); TODO:

   vuelo_actual->num_vuelo = -1;
   equipajes_cargados = vuelo_actual->cant_equipajes;
	mutex_checkin.unlock();
   return equipajes_cargados;
}

void ApiCheckIn::comienza_checkin_pasajero() {
	mutex_checkin.lock();
}

void ApiCheckIn::fin_checkin_pasajero() {
	mutex_checkin.unlock();
}

void ApiCheckIn::registrar_equipaje(Equipaje& equipaje) {
	if (vuelo_actual->num_vuelo == -1)
		throw std::runtime_error("Registrando equipaje en puesto_checkin sin vuelo asignado");

   vuelo_actual->cant_equipajes++;
	cinta_checkin_out.poner_equipaje(equipaje, id_checkin);
}

int ApiCheckIn::get_vuelo_actual() {

	if (vuelo_actual->num_vuelo == -1)
		throw PuestoCheckinSinVueloAsignado(id_checkin);
	return vuelo_actual->num_vuelo;
}

void ApiCheckIn::recibir_pasajero_para_checkin(int& id_pasajero, std::vector<Equipaje>& equipajes) {
	tMensajePasajeroCheckin msg;
	msg.mtype = (long) id_checkin;

	queue_pasajeros->pull(&msg, sizeof(tMensajePasajeroCheckin)-sizeof(long), (long) id_checkin);

	id_pasajero = msg.id_pasajero;
	for (int i = 0; i < msg.cant_equipajes; i++) {
		equipajes.push_back(msg.equipajes[i]);
	}
}

void ApiCheckIn::recibir_mensaje_controlador_checkin(tMeansajeCheckin& msg_checkin) {
   Log::debug("recibir_mensaje_controlador_checkin");
   //MessageQueue queue_checkin(std::string(path_to_locks).append(PATH_COLA_CONTROL_CHECKIN).c_str(), 0);
   queue_controlador->pull(&msg_checkin, sizeof(tMeansajeCheckin)-sizeof(long), id_checkin);
}

int ApiCheckIn::get_cinta_checkin() {
   return vuelo_actual->id_cinta_checkin;
}
