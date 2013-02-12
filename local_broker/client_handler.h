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
	std::string directorio_de_trabajo;
	MessageQueue cola_token_manager;
	Grupo * grupo;
	std::string nombre_cliente;

	void registrar_cliente(const std::string & nombre_cliente);
	void join_group(const std::string & grupo);
	void leave_group();
	void loop_token();

public:
	ClientHandler(const std::string & directorio_de_trabajo, char id, int fd);
	virtual ~ClientHandler();

	void run();
};

#endif /* CLIENTHANDLER_H_ */
