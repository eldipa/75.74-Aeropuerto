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
#include "oserror.h"
#include "local_broker_constants.h"
#include <csignal>
#include <unistd.h>
#include "daemon.h"

#define MAX_CLIENTES 100

static char num_socket [11];
static char id_cola_token_manager [2];
static char directorio_de_trabajo [200];
static char groups_file [200];
static char * args_client_handler [] = {
	(char*)"client_handler", directorio_de_trabajo, num_socket, id_cola_token_manager, NULL};

#define LANZAR_TOKEN_MANAGER 1

#if LANZAR_TOKEN_MANAGER == 1
static char * args_token_manager [] = {
	(char*)"token_manager", directorio_de_trabajo, id_cola_token_manager, groups_file, NULL};
#endif

LocalBroker::LocalBroker(const std::string & directorio, const std::string & groups,const std::string & servicio)
	: server_socket(true)
{

	server_socket.source(servicio);

	strncpy(directorio_de_trabajo, directorio.c_str(), 200);
	strncpy(groups_file, groups.c_str(), 200);

}

LocalBroker::~LocalBroker() {
	std::vector<Process *> :: iterator i;
	for(i = hijos.begin();i!=hijos.end();i++){
		(*i)->send_signal(SIGTERM,false);
		(*i)->wait();
		delete (*i);
	}
}

void LocalBroker::run() {
	//char debug [200];
	int new_socket;
	bool exit = 0;
	Process * handler;
	long cantidad_de_clientes = 0;
#if LANZAR_TOKEN_MANAGER == 1
	Process * p = new Process("token_manager",args_token_manager);
	hijos.push_back(p);
#endif

	ignore_signals();

	do {
		try {
			new_socket = server_socket.listen_fd(10);

			snprintf(num_socket, 10, "%d", new_socket);
			snprintf(id_cola_token_manager, 2, "%d", 0);

			//new_socket->receivesome(debug, 200);

			//snprintf(debug, 200, "%s", "RECIBIDO!");

			//new_socket->sendsome(debug, strlen(debug));

			//std::cout << "creando handler " << new_socket << std::endl;

			handler = new Process("client_handler", args_client_handler);

			hijos.push_back(handler);

			close(new_socket);

			Log::info("Conectado cliente numero %d",cantidad_de_clientes++);
			//exit = true;
		} catch (OSError & error) {
			exit = 1;
		}
	} while (!exit);
	//p.wait();

}
