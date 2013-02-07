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

char debug [200];

void procesar_peticion(Socket & socket, mensajes::mensajes_local_broker_t & mensaje) {

	switch (mensaje.peticion) {
		case mensajes::REGISTER:
			// chequear que la aplicacion no esté logueada actualmente
			mensaje.respuesta = mensajes::OK;
			// generar id de cliente/conexion
			break;
		case mensajes::JOIN:
			// buscar el grupo en el que está el recurso, si no existe crearlo
			mensaje.respuesta = mensajes::OK;
			break;
		case mensajes::LEAVE:
			// sacar al cliente del grupo
			break;
		default:
			break;
	}
	socket.sendsome(&mensaje, sizeof(mensajes::mensajes_local_broker_t));
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

	Socket socket(fd);

	/*socket.receivesome(debug, 200);
	 std::cout << "Recibido: " << debug << std::endl;
	 snprintf(debug, 200, "%s", "RECIBIDO!");
	 socket.sendsome(debug, strlen(debug));*/

	mensajes::mensajes_local_broker_t mensaje;

	do {
		socket.receivesome(&mensaje, sizeof(mensajes::mensajes_local_broker_t));

		procesar_peticion(socket, mensaje);

	} while (mensaje.peticion != mensajes::LEAVE && mensaje.peticion != mensajes::END_JOIN);

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
