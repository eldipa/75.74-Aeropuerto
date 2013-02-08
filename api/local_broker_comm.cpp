/*
 * LocalBrokerComm.cpp
 *
 *  Created on: 05/02/2013
 *      Author: gonzalo
 */

#include "local_broker_comm.h"
#include <cstdio>
#include <cstring>
#include <iostream>
#include "genericerror.h"
#include "mensajes_de_red.h"

LocalBrokerComm::LocalBrokerComm(const std::string & app_name, const std::string & broker_hostname)
	: socket_broker(true)
{
	
	// Establezco conexion con el servidor
	socket_broker.destination(broker_hostname.c_str(), "1234");

	mensajes::mensajes_local_broker_t mensaje;

	mensaje.peticion = mensajes::REGISTER;

	bzero(mensaje.datos, sizeof(mensaje.datos));

	snprintf(mensaje.datos, sizeof(mensaje.datos), "%s", app_name.c_str());

	// Hago un pedido de registro
	socket_broker.sendsome(&mensaje, sizeof(mensajes::mensajes_local_broker_t));

	socket_broker.receivesome(&mensaje, sizeof(mensajes::mensajes_local_broker_t));

	if (mensaje.respuesta == mensajes::ERROR) {
		throw GenericError("Register Error: %s", mensaje.datos);
	}

}

LocalBrokerComm::~LocalBrokerComm() {

}

void LocalBrokerComm::join(const std::string & nombre_recurso) {
	std::string name;
	name = nombre_recurso;
	name = name + "";
}

void LocalBrokerComm::leave() {

}

void LocalBrokerComm::wait_mutex(const std::string & nombre_recurso) {
	std::string name;
	name = nombre_recurso;
	name = name + "";
}

void LocalBrokerComm::free_mutex(const std::string & nombre_recurso) {
	std::string name;
	name = nombre_recurso;
	name = name + "";
}
