/*
 * client_handler.cpp
 *
 *  Created on: 05/02/2013
 *      Author: gonzalo
 */

/*
 * local_broker_launcher.cpp
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

ClientHandler::ClientHandler(const std::string & directorio_de_trabajo, int fd)
	: socket(fd)
{
	std::string name;
	name = directorio_de_trabajo;
	name = name + "";
}

ClientHandler::~ClientHandler() {
	// TODO Auto-generated destructor stub
}

void ClientHandler::procesar_peticion(mensajes::mensajes_local_broker_t & mensaje) {

	switch (mensaje.peticion) {
		case mensajes::REGISTER:
			std::cout << "Register: " << mensaje.datos << std::endl;
			// chequear que la aplicacion no esté logueada actualmente
			mensaje.respuesta = mensajes::OK;
			// generar id de cliente/conexion
			break;
		case mensajes::JOIN:
			std::cout << "Join: " << mensaje.datos << std::endl;
			// buscar el grupo en el que está el recurso, si no existe crearlo
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

void ClientHandler::run() {
	mensajes::mensajes_local_broker_t mensaje;

	do {
		try {

			socket.receivesome(&mensaje, sizeof(mensajes::mensajes_local_broker_t));

		} catch (const std::exception & e) {
			std::cerr << "Catch OK" << std::endl;
			mensaje.peticion = mensajes::LEAVE;
		}

		this->procesar_peticion(mensaje);

	} while (mensaje.peticion != mensajes::LEAVE && mensaje.peticion != mensajes::END_JOIN);
}

int main(int argc, char * argv [])
try
{
	if (argc != 2) {
		std::cerr << "Falta el socket" << std::endl;
		return -1;
	}
	int fd;

	fd = atoi(argv [1]);

	ClientHandler handler("./locks", fd);

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
