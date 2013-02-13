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
	mem_local = NULL;
}

ClientHandler::~ClientHandler() {
	if (mem_local) {
		delete [] mem_local;
	}
}

//void ClientHandler::registrar_cliente(const std::string & nombre_cliente){
//}

void ClientHandler::join_group(const std::string & nombre_grupo) {
	size_t tamanio;
	// Obtengo la memoria compartida del grupo
	grupo = new Grupo(directorio_de_trabajo, nombre_grupo);
	// Agrego el cliente y el numero de la cola
	grupo->join(nombre_cliente.c_str());

	// calculo la cantidad de bloques a enviar por token
	tamanio = grupo->get_mem_size();

	cantidad_de_bloques_a_enviar = (tamanio / DATA_SIZE);

	if (tamanio % DATA_SIZE != 0 || tamanio == 0) {
		cantidad_de_bloques_a_enviar++;
	}

	tamanio_memoria = tamanio;

	if (tamanio != 0) {
		mem_local = new char [tamanio];
	}
}

void ClientHandler::leave_group() {
	// Saco al cliente del grupo
	grupo->leave(nombre_cliente.c_str());
}

void ClientHandler::procesar_peticion(mensajes::mensajes_local_broker_t & mensaje) {

	switch (mensaje.peticion) {
		case mensajes::REGISTER:
			//std::cout << "Register: " << mensaje.datos << std::endl;
			// chequear que la aplicacion no esté logueada actualment
			//registrar_cliente(mensaje.datos);
			// generar id de cliente/conexion
			nombre_cliente.assign(mensaje.datos);
			break;
		case mensajes::JOIN:
			//std::cout << "Join: " << mensaje.datos << std::endl;
			// buscar el grupo en el que está el recurso, si no existe crearlo
			join_group(mensaje.datos);
#ifdef __x86_64__
			snprintf(mensaje.datos, DATA_SIZE, "%lu:%lu", this->cantidad_de_bloques_a_enviar, this->tamanio_memoria);
#else
			snprintf(mensaje.datos, DATA_SIZE, "%u:%u", this->cantidad_de_bloques_a_enviar, this->tamanio_memoria);
#endif
			mensaje.respuesta = mensajes::OK;
			break;
		case mensajes::LEAVE:
			//std::cout << "Leave: " << mensaje.datos << std::endl;
			// sacar al cliente del grupo
			break;
		default:
			break;
	}
	socket.sendsome(&mensaje, sizeof(mensajes::mensajes_local_broker_t));
}

void ClientHandler::send_token() {
	mensajes::mensajes_local_broker_token_t mensaje;
	size_t i;
	for (i = 0; i < this->cantidad_de_bloques_a_enviar ; i++) {
		strncpy(mensaje.recurso, grupo->get_nombre_recurso().c_str(), MAX_NAME_SIZE);
		if (tamanio_memoria != 0) {
			memcpy(mensaje.datos, ((char *)grupo->memory_pointer() + i * DATA_SIZE),
				std::min(this->tamanio_memoria - i * DATA_SIZE, size_t(DATA_SIZE)));
		}
		socket.sendsome(&mensaje, sizeof(mensajes::mensajes_local_broker_token_t));
	}
}

size_t ClientHandler::recv_token() {
	mensajes::mensajes_local_broker_token_t mensaje;
	size_t cant = 0;
	size_t leidos;
	size_t i;

	bzero(mem_local, grupo->get_mem_size());
	for (i = 0; i < this->cantidad_de_bloques_a_enviar ; i++) {
		cant = 0;
		do {
			leidos = socket.receivesome(((char *)&mensaje) + cant,
				sizeof(mensajes::mensajes_local_broker_token_t) - cant);
			if (leidos == 0) {
				return 0;
			}
			cant += leidos;
		} while (cant < sizeof(mensajes::mensajes_local_broker_token_t));
		if (tamanio_memoria != 0) {
			memcpy(mem_local + i * DATA_SIZE, mensaje.datos, std::min(tamanio_memoria, size_t(DATA_SIZE)));
		}
	}
	// actualizo la memoria con lo recibido
	if (tamanio_memoria != 0) {
		memcpy(grupo->memory_pointer(), mem_local, grupo->get_mem_size());
	}
	return this->cantidad_de_bloques_a_enviar * DATA_SIZE;
}

void ClientHandler::loop_token() {
	bool leave = false;
	bool tengo_el_token = false;
	/*int a;
	 char data [DATA_SIZE];*/
	do {
		try {
			// espero el token
			grupo->lock_token();
			tengo_el_token = true;
			/*std::cout << (char*)grupo->memory_pointer() << std::endl;
			 sscanf((char*)grupo->memory_pointer(), "%[^:]:%d", data, &a);
			 a++;
			 snprintf((char*)grupo->memory_pointer(), DATA_SIZE, "%s:%d", "client_handler", a);
			 std::cout << (char*)grupo->memory_pointer() << std::endl;*/

			// envio el token al cliente
			send_token();

			// espero que devuelva el token
			if (recv_token() == 0) {
				leave = true;
			}
		} catch (OSError & error) {
			leave = true;
		}
		if (leave) {
			// tengo que salir del grupo antes de liberar el token
			leave_group();
		}
		if (tengo_el_token) {
			// libero el token
			grupo->release_token(&cola_token_manager);
		}
		tengo_el_token = false;
	} while (!leave);

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
