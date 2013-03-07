/*
 * LocalBrokerComm.h
 *
 *  Created on: 05/02/2013
 *      Author: gonzalo
 */

#ifndef LOCALBROKERCOMM_H_
#define LOCALBROKERCOMM_H_

#include <string>
#include "socket.h"
#include "mensajes_de_red.h"
#include <csignal>

class LocalBrokerComm {
private:

	Socket socket_broker;
	size_t cantidad_de_bloques;
	size_t tamanio_memoria;
	std::string nombre_aplicacion;
	std::string nombre_grupo;
	bool tengo_token;

public:
	LocalBrokerComm(const std::string & app_name, const std::string & broker_hostname, const std::string & service);
	virtual ~LocalBrokerComm();

	void join(const std::string & nombre_recurso, mensajes::peticiones_t tipo_join);
	void leave();

	void wait_mutex(void * memory);
	void free_mutex(void * memory);
	void set_tiene_token(bool t);
	void set_salir();

	size_t get_mem_size();
	static sig_atomic_t salir;

	bool tiene_token();
};

#endif /* LOCALBROKERCOMM_H_ */
