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
#include <cstdlib>
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
	mensajes::mensajes_local_broker_t mensaje;
	mensaje.peticion = mensajes::JOIN;
	strncpy(mensaje.datos, nombre_recurso.c_str(), std::min(nombre_recurso.size(), size_t(MAX_NAME_SIZE)));

	socket_broker.sendsome(&mensaje, sizeof(mensajes::mensajes_local_broker_t));

	socket_broker.receivesome(&mensaje, sizeof(mensajes::mensajes_local_broker_t));

	if (mensaje.respuesta == mensajes::ERROR) {
		throw GenericError("Join Error: %s", mensaje.datos);
	} else {
		sscanf(mensaje.datos, "%lu", &this->tamanio_memoria);
	}
}

void LocalBrokerComm::leave() {
	socket_broker.disassociate();
}

void LocalBrokerComm::wait_mutex(void * memory) {
	size_t leidos;
	mensajes::mensajes_local_broker_token_t mensaje;
	leidos = socket_broker.receivesome(&mensaje, sizeof(mensajes::mensajes_local_broker_token_t));

	while (leidos < sizeof(mensajes::mensajes_local_broker_token_t)) {
		leidos += socket_broker.receivesome(&mensaje + leidos, sizeof(mensajes::mensajes_local_broker_token_t) - leidos);
	}

	memcpy(memory, mensaje.datos, this->tamanio_memoria);
}

void LocalBrokerComm::free_mutex(void * memory) {
	mensajes::mensajes_local_broker_token_t mensaje;
	memcpy(mensaje.datos, memory, this->tamanio_memoria);
	socket_broker.sendsome(&mensaje, sizeof(mensajes::mensajes_local_broker_token_t));
}
