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
#include "signalhandler.h"
#include "eventhandler.h"
#include <vector>
#include "memoria_distribuida.h"
#include "mutex_distribuido.h"
#include "semaphore_set_distribuido.h"
#include "yasper.h"

typedef enum {
	SHAREDMEMORY, MUTEX, SEMAFORO
} tipo_ipc_t;

class LocalBrokerComm: public EventHandler {
private:

	yasper::ptr<Socket> socket_broker;
	size_t cantidad_de_bloques;
	size_t tamanio_memoria;
	std::string nombre_aplicacion;
	std::string nombre_grupo;

	yasper::ptr<MemoriaDistribuida> memoria;
	yasper::ptr<MutexDistribuido> mutex;
	yasper::ptr<SemaphoreSetDistribuido> semaforo;
	int numero_de_semaforo;
	tipo_ipc_t tipo_de_recurso;

	std::vector<std::string> lista_de_brokers;
	std::vector<std::string> lista_de_servicios;

	bool conectado;
	size_t numero_broker_conectado;

	mensajes::mensajes_local_broker_token_t mensaje;

	ControlTokens * control;

	sig_atomic_t salir;
	sig_atomic_t tengo_token;

	void registrar();
	void join();
	void reconectar();

	virtual void handleSignal(int signum);

	void loop_memoria();
	void loop_mutex();
	void loop_mutex_padre();
	void loop_mutex_hijo();
	void loop_semaforo();
	void loop_semaforo_padre();
	void loop_semaforo_hijo();

	void wait_token();
	void send_token();
	void wait_token(void * memory);
	void send_token(void * memory);

	void leave();

public:
	LocalBrokerComm(const std::string & directorio_de_trabajo, const std::string & app_name,
		const std::string & group_name, const std::vector<std::string> & broker_hostnames,
		const std::vector<std::string> & services, tipo_ipc_t tipo, int numero_de_semaforo = 0,
		int cantidad_de_semaforos = 0);
	virtual ~LocalBrokerComm();

	void run();

};

#endif /* LOCALBROKERCOMM_H_ */
