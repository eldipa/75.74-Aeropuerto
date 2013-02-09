/*
 * LocalBroker.cpp
 *
 *  Created on: 05/02/2013
 *      Author: gonzalo
 */

#include "local_broker.h"

#include <memory>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include "process.h"
#include "local_broker_constants.h"

#define MAX_CLIENTES 100
#define TAMANIO_SHM 1000

static char num_socket [11];
static char * args_client_handler [] = {(char*)"client_handler", num_socket, NULL};

LocalBroker::LocalBroker(const std::string & directorio_de_trabajo)
	: server_socket(true), tabla_clientes_locales(directorio_de_trabajo, true)
{
	std::string puerto("1234");

	server_socket.source(puerto);

}

LocalBroker::~LocalBroker() {

}

void LocalBroker::run() {
	//char debug [200];
	int new_socket;

	new_socket = server_socket.listen_fd(10);

	snprintf(num_socket, 10, "%d", new_socket);

	//new_socket->receivesome(debug, 200);

	//snprintf(debug, 200, "%s", "RECIBIDO!");

	//new_socket->sendsome(debug, strlen(debug));

	//std::cout << "creando handler " << new_socket << std::endl;

	Process p("client_handler", args_client_handler);

	//p.wait();

}