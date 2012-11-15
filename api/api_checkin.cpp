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

ApiCheckIn::ApiCheckIn(int id_checkin, const char* path_to_locks, int id_cinta_checkin) : 
   id_checkin(id_checkin), id_cinta_checkin(id_cinta_checkin)
{
    snprintf(path_to_torre_de_control_lock, 128, "%s%s", path_to_locks, PATH_TORRE_DE_CONTROL);
    snprintf(path_to_cinta_checkin_lock, 128, "%s%s", path_to_locks, PATH_CINTA_CHECKIN);
    snprintf(path_to_puesto_checkin_lock, 128, "%s%s", path_to_locks, PATH_PUESTO_CHECKIN);
    snprintf(path_to_control_checkin_lock, 128, "%s%s", path_to_locks, PATH_COLA_CONTROL_CHECKIN);

    sem_set = std::auto_ptr<SemaphoreSet>(new SemaphoreSet(path_to_puesto_checkin_lock, id_checkin*cant_ipcs,1));
    mutex_checkin = std::auto_ptr<Mutex>(new Mutex(*sem_set,0));
    queue_pasajeros = std::auto_ptr<MessageQueue>(new MessageQueue(path_to_puesto_checkin_lock, id_checkin*cant_ipcs+1));
    vuelo_actual = std::auto_ptr< SharedObject<int> >(new SharedObject<int>(path_to_puesto_checkin_lock, id_checkin*cant_ipcs+2, 0664));

}

ApiCheckIn::~ApiCheckIn() {
}

int cantidad_vuelos_trasbordo_a(int numero_vuelo) {
	Database db("aeropuerto", true);

	std::auto_ptr<Statement> query = db.statement(
			"select count(vuelo_destino) from VuelosIntercargo where vuelo_origen = :vuelo");
	query->set(":vuelo", numero_vuelo);

	return (query->begin()->at<int>(0));
}


void ApiCheckIn::iniciar_checkin( int numero_vuelo ) {
   mutex_checkin->lock();
   if( (**vuelo_actual) == -1 ) {
      MessageQueue checkin(path_to_torre_de_control_lock, Q_CHECKINS_HABILITADOS);
   
      Log::info("Notificando checkin abierto para vuelo %i", numero_vuelo);
      // Envio un mensaje por cada vuelo de trasbordo
      for (int i = 0; i < cantidad_vuelos_trasbordo_a(numero_vuelo); i ++ )
         checkin.push(&numero_vuelo, sizeof(numero_vuelo));

      // Actualizo la info sobre el vuelo actual
      **vuelo_actual = numero_vuelo;
   } else {
      Log::crit("El checkin ya esta abierto en el puesto %d para el vuelo %d", id_checkin, **vuelo_actual);
   }
   mutex_checkin->unlock();
}

void ApiCheckIn::cerrar_checkin() {
	mutex_checkin->lock();

	if (**vuelo_actual == -1) {
		throw std::runtime_error("No había ningún checkin abierto");
	}
	//MessageQueue checkin(path_to_torre_de_control_lock, Q_CHECKINS_CERRADO);


   if (**vuelo_actual == -1) {
      throw std::runtime_error("No había ningún checkin abierto");
   }
   //MessageQueue checkin(path_to_torre_de_control_lock, Q_CHECKINS_CERRADO);

   Log::info("Notificando checkin cerrado para vuelo %i", **vuelo_actual);
   //checkin.push(&vuelo_actual, sizeof(vuelo_actual)); TODO:
    
   **vuelo_actual = -1;

	mutex_checkin->unlock();
}

void ApiCheckIn::comienza_checkin_pasajero() {
	mutex_checkin->lock();
}

void ApiCheckIn::fin_checkin_pasajero() {
	mutex_checkin->unlock();
}

void ApiCheckIn::registrar_equipaje(Equipaje& equipaje) {
	if (**vuelo_actual == -1)
		throw std::runtime_error("Registrando equipaje en puesto_checkin sin vuelo asignado");

	CintaCheckin cinta_checkin_out(path_to_cinta_checkin_lock, id_cinta_checkin);
	cinta_checkin_out.poner_equipaje(equipaje, id_checkin);
}

int ApiCheckIn::get_vuelo_actual() {

	if (**vuelo_actual == -1)
		throw PuestoCheckinSinVueloAsignado(id_checkin);
	return (**vuelo_actual);
}

void ApiCheckIn::recibir_pasajero_para_checkin(int& id_pasajero, std::vector<Equipaje>& equipajes) {
	tMensajePasajeroCheckin msg;
	msg.mtype = (long) id_checkin;

	queue_pasajeros->pull(&msg, sizeof(tMensajePasajeroCheckin), (long) id_checkin);

	id_pasajero = msg.id_pasajero;
	for (int i = 0; i < msg.cant_equipajes; i++) {
		equipajes.push_back(msg.equipajes[i]);
	}
}

void ApiCheckIn::llego_pasajero_para_checkin(int id_pasajero,
		const std::vector<Equipaje>& equipajes) {
	tMensajePasajeroCheckin msg;
	msg.mtype = (long) id_checkin;
	msg.id_pasajero = id_pasajero;
	msg.cant_equipajes = 0;

	if (equipajes.size() > MAX_EQUIPAJES_POR_PERSONA)
		throw std::runtime_error(
				"llego_pasajero_para_checkin:intentando checkin con mas vliajs que MAX_EQUIPAJES_POR_PERSONA");

	std::vector<Equipaje>::const_iterator it = equipajes.begin();
	for (; it != equipajes.end(); it++) {
		msg.equipajes[msg.cant_equipajes] = (*it);
		msg.cant_equipajes++;
	}

	queue_pasajeros->push(&msg, sizeof(tMensajePasajeroCheckin));
}

void ApiCheckIn::recibir_mensaje_controlador_checkin(tMeansajeCheckin& msg_checkin) {
   Log::debug("recibir_mensaje_controlador_checkin path=%s", path_to_control_checkin_lock);
   MessageQueue queue_checkin(path_to_control_checkin_lock, 0);
   queue_checkin.pull(&msg_checkin, sizeof(tMeansajeCheckin));
}

