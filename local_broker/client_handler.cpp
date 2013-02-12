/*
 * client_handler.cpp
 *
 *  Created on: 05/02/2013
 *      Author: gonzalo
 */

#include <iostream>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include "log.h"
#include "local_broker.h"
#include "mensajes_de_red.h"
#include "messagequeue.h"
#include "oserror.h"

#include "client_handler.h"

ClientHandler::ClientHandler(const std::string & directorio_de_trabajo, char id, int fd)
	: socket(fd), directorio_de_trabajo(directorio_de_trabajo),
		cola_token_manager(std::string(directorio_de_trabajo).append(PATH_COLA_TOKEN_MANAGER).c_str(), id)
{
	std::string name;
	name = directorio_de_trabajo;
	name = name + "";
}

ClientHandler::~ClientHandler() {
}

//void ClientHandler::registrar_cliente(const std::string & nombre_cliente){

//}

void ClientHandler::join_group(const std::string & nombre_grupo) {

	// Obtengo la memoria compartida del grupo
	grupo = new Grupo(directorio_de_trabajo, nombre_grupo);
	// Agrego el cliente y el numero de la cola
	grupo->join(nombre_cliente.c_str());
}

void ClientHandler::leave_group() {
	// Saco al cliente del grupo
	grupo->leave(nombre_cliente.c_str());
}

void ClientHandler::procesar_peticion(mensajes::mensajes_local_broker_t & mensaje) {

	switch (mensaje.peticion) {
		case mensajes::REGISTER:
			std::cout << "Register: " << mensaje.datos << std::endl;
			// chequear que la aplicacion no esté logueada actualment
			//registrar_cliente(mensaje.datos);
			// generar id de cliente/conexion
			nombre_cliente.assign(mensaje.datos);
			break;
		case mensajes::JOIN:
			std::cout << "Join: " << mensaje.datos << std::endl;
			// buscar el grupo en el que está el recurso, si no existe crearlo
			join_group(mensaje.datos);
			snprintf(mensaje.datos, DATA_SIZE, "%lu", grupo->get_mem_size());
			mensaje.respuesta = mensajes::OK;
			break;
		case mensajes::LEAVE:
			std::cout << "Leave: " << mensaje.datos << std::endl;
			// sacar al cliente del grupo
			break;
		default:
			break;
	}
	socket.sendsome(&mensaje, sizeof(mensajes::mensajes_local_broker_t));
}

void ClientHandler::loop_token() {
	mensajes::mensajes_local_broker_token_t mensaje;
	bool leave = false;
	int a;
	char data [DATA_SIZE];
	do {
		try {
			// espero el token
			grupo->lock_token();
			mensaje.cant_bytes_total = grupo->get_mem_size();

			std::cout << (char*)grupo->memory_pointer() << std::endl;
			sscanf((char*)grupo->memory_pointer(), "%[^:]:%d", data, &a);
			a++;
			snprintf((char*)grupo->memory_pointer(), DATA_SIZE, "%s:%d", "client_handler", a);
			std::cout << (char*)grupo->memory_pointer() << std::endl;

			// envio el token al cliente
			memcpy(mensaje.datos, grupo->memory_pointer(), grupo->get_mem_size());
			socket.sendsome(&mensaje, sizeof(mensajes::mensajes_local_broker_token_t));

			// espero que devuelva el token
			if (socket.receivesome(&mensaje, sizeof(mensajes::mensajes_local_broker_token_t)) != 0) {
				// actualizo la memoria con lo recibido
				memcpy(grupo->memory_pointer(), mensaje.datos, grupo->get_mem_size());

				sscanf((char*)grupo->memory_pointer(), "%[^:]:%d", data, &a);
				a++;
				snprintf((char*)grupo->memory_pointer(), DATA_SIZE, "%s:%d", "client_handler", a);
				std::cout << (char*)grupo->memory_pointer() << std::endl;
			} else {
				leave = true;
			}

		} catch (OSError & error) {
			leave = true;
		}

		// libero el token
		grupo->release_token(&cola_token_manager);
	} while (!leave);

	leave_group();

}

void ClientHandler::run() {
	mensajes::mensajes_local_broker_t mensaje;

	do {

		if (socket.receivesome(&mensaje, sizeof(mensajes::mensajes_local_broker_t)) == 0) {
			mensaje.peticion = mensajes::LEAVE;
		}

		this->procesar_peticion(mensaje);

	} while (mensaje.peticion != mensajes::LEAVE && mensaje.peticion != mensajes::JOIN);

	loop_token();

}

int main(int argc, char * argv [])
try
{
	if (argc != 4) {
		std::cerr << "Falta el socket" << std::endl;
		return -1;
	}
	int fd;
	char id;

	std::cout << "client_handler " << argv [1] << " " << argv [2] << " " << argv [3] << std::endl;

	fd = atoi(argv [2]);
	id = atoi(argv [3]);

	///// SOCKET ESCUCHA SOLO PARA DEBUG
	/*int new_socket;
	 std::string puerto("1234");
	 Socket * server_socket = new Socket(true);
	 server_socket->source(puerto);
	 new_socket = server_socket->listen_fd(10);
	 fd = new_socket;
	 server_socket->disassociate();
	 delete server_socket;*/

	ClientHandler handler(argv [1], id, fd);

	handler.run();

	/*socket.receivesome(debug, 200);
	 std::cout << "Recibido: " << debug << std::endl;
	 snprintf(debug, 200, "%s", "RECIBIDO!");
	 socket.sendsome(debug, strlen(debug));*/

}
catch (const std::exception & e) {
	std::cerr << "Error Broker Local" << std::endl;
	std::cerr << e.what() << std::endl;
	Log::crit("%s", e.what());
}
catch (...) {
	std::cerr << "Error Broker Local" << std::endl;
	Log::crit("Critical error. Unknow exception at the end of the 'main' function.");
}
