/*
 * ClientHandler.h
 *
 *  Created on: 07/02/2013
 *      Author: gonzalo
 */

#ifndef CLIENTHANDLER_H_
#define CLIENTHANDLER_H_

#include "mensajes_de_red.h"
#include "messagequeue.h"
#include "socket.h"

class ClientHandler {
private:
	void procesar_peticion(mensajes::mensajes_local_broker_t & mensaje);
	Socket socket;

	MessageQueue * cola_token_manager;

public:
	ClientHandler(const std::string & directorio_de_trabajo, int fd);
	virtual ~ClientHandler();

	void run();
};

#endif /* CLIENTHANDLER_H_ */
