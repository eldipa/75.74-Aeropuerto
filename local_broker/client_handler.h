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
#include "grupo.h"
#include "socket.h"
#include "signalhandler.h"
#include "eventhandler.h"

class ClientHandler: EventHandler {
private:
	void procesar_peticion(mensajes::mensajes_local_broker_t & mensaje);
	Socket socket;
	std::string directorio_de_trabajo;
	MessageQueue cola_token_manager;
	Grupo * grupo;
	std::string nombre_cliente;
	std::string nombre_grupo;
	char * mem_local;

	size_t cantidad_de_bloques_a_enviar;
	size_t tamanio_memoria;
	mensajes::peticiones_t tipo_join;

	int * cantidad_clientes_esperando;
	char * grupos_creados [MAX_GRUPOS];

	mensajes::mensajes_local_broker_token_t mensaje;

	sig_atomic_t salir;
	sig_atomic_t tengo_token;
	sig_atomic_t chequear_asignacion;

	void avisar_creacion_grupo(const std::string & nombre_grupo);
	void registrar_cliente(const std::string & nombre_cliente);
	void join_group(const std::string & grupo);
	void leave_group();
	void loop_semaforo_padre();
	void chequear_status_salida_hijo();
	void loop_semaforo_hijo();
	void loop_semaforo();
	void loop_memoria();
	void loop_token();

	virtual void handleSignal(int signum);

	void send_token_sem();
	void recv_token_sem();

	void send_token();
	size_t recv_token();

	bool leave;

public:
	ClientHandler(const std::string & directorio_de_trabajo, /*char id,*/int fd);
	virtual ~ClientHandler();

	void run();
};

#endif /* CLIENTHANDLER_H_ */
