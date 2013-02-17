/*
 * LocalBrokerComm.cpp
 *
 *  Created on: 05/02/2013
 *      Author: gonzalo
 */

#include "local_broker_comm.h"
#include <cstdio>
#include <cstring>
#include <iostream>
#include <cstdlib>
#include "genericerror.h"
#include "mensajes_de_red.h"
#include "control_tokens.h"
#include "memoria_distribuida.h"
#include "mutex_distribuido.h"

LocalBrokerComm::LocalBrokerComm(const std::string & app_name, const std::string & broker_hostname,
	const std::string & service)
	: socket_broker(true)
{
	
	// Establezco conexion con el servidor
	socket_broker.destination(broker_hostname.c_str(), service);

	mensajes::mensajes_local_broker_t mensaje;

	mensaje.peticion = mensajes::REGISTER;

	bzero(mensaje.datos, sizeof(mensaje.datos));

	snprintf(mensaje.datos, sizeof(mensaje.datos), "%s", app_name.c_str());

	// Hago un pedido de registro
	socket_broker.sendsome(&mensaje, sizeof(mensajes::mensajes_local_broker_t));

	socket_broker.receivesome(&mensaje, sizeof(mensajes::mensajes_local_broker_t));

	if (mensaje.respuesta == mensajes::ERROR) {
		throw GenericError("Register Error: %s", mensaje.datos);
	}

}

LocalBrokerComm::~LocalBrokerComm() {

}

void LocalBrokerComm::join(const std::string & nombre_recurso) {
	mensajes::mensajes_local_broker_t mensaje;
	mensaje.peticion = mensajes::JOIN;
	strncpy(mensaje.datos, nombre_recurso.c_str(), std::min(nombre_recurso.size(), size_t(MAX_NOMBRE_RECURSO)));

	socket_broker.sendsome(&mensaje, sizeof(mensajes::mensajes_local_broker_t));

	socket_broker.receivesome(&mensaje, sizeof(mensajes::mensajes_local_broker_t));

	if (mensaje.respuesta == mensajes::ERROR) {
		throw GenericError("Join Error: %s", mensaje.datos);
	} else {
#ifdef __x86_64__
		sscanf(mensaje.datos, "%lu:%lu", &this->cantidad_de_bloques, &this->tamanio_memoria);
#else
		sscanf(mensaje.datos, "%u:%u", &this->cantidad_de_bloques, &this->tamanio_memoria);
#endif
	}
}

void LocalBrokerComm::leave() {
	socket_broker.disassociate();
}

void LocalBrokerComm::wait_mutex(void * memory) {
	size_t leidos;
	mensajes::mensajes_local_broker_token_t mensaje;
	size_t cant = 0;
	size_t i;

	for (i = 0; i < this->cantidad_de_bloques ; i++) {
		cant = 0;
		do {
			leidos = socket_broker.receivesome((char *)&mensaje + cant,
				sizeof(mensajes::mensajes_local_broker_token_t) - cant);
			if (leidos == 0) {
				//return 0;
				throw GenericError("Conexión con Broker Rota");
			}
			cant += leidos;
		} while (cant < sizeof(mensajes::mensajes_local_broker_token_t));
		if (tamanio_memoria != 0 && memory != NULL) {
			memcpy((char *)memory + i * DATA_SIZE, mensaje.datos, std::min(tamanio_memoria, size_t(DATA_SIZE)));
		}
	}

}

void LocalBrokerComm::free_mutex(void * memory) {
	//mensajes::mensajes_local_broker_token_t mensaje;
	//memcpy(mensaje.datos, memory, this->cantidad_de_bloques);
	//socket_broker.sendsome(&mensaje, sizeof(mensajes::mensajes_local_broker_token_t));

	mensajes::mensajes_local_broker_token_t mensaje;
	size_t i;
	for (i = 0; i < this->cantidad_de_bloques ; i++) {
		if (tamanio_memoria != 0 && memory != NULL) {
			memcpy(mensaje.datos, ((char *)memory + i * DATA_SIZE),
				std::min(this->tamanio_memoria - i * DATA_SIZE, size_t(DATA_SIZE)));
		}
		socket_broker.sendsome(&mensaje, sizeof(mensajes::mensajes_local_broker_token_t));
	}
}

size_t LocalBrokerComm::get_mem_size() {
	return this->tamanio_memoria;
}

int main(int argc, char * argv []) {
	char hostname [100];
	char service [10];
	char grupo [MAX_NOMBRE_RECURSO];
	size_t tamanio;
	char id;

	// maneja la comunicacion con el broker del lado del cliente
	if (argc < 4) {
		std::cerr << "Faltan argumentos" << std::endl;
		std::cerr << "1: directorio de trabajo" << std::endl;
		std::cerr << "2: nombre de la aplicacion" << std::endl;
		std::cerr << "3: direccion del broker local (HOSTNAME:PUERTO)" << std::endl;
		std::cerr << "4: nombre del grupo" << std::endl;
		std::cerr << "5: id" << std::endl;
		std::cerr << "6: tamanio mem compartida" << std::endl;
		return -1;
	}

	std::cout << argv[0];
	for(int i =1 ; i < argc ; i++){
		std::cout << " "<< argv[i];
	}
	std::cout << std::endl;

	sscanf(argv [3], "%[^:]:%[^:]", hostname, service);
	strncpy(grupo, argv [4], MAX_NOMBRE_RECURSO);
	id = atoi(argv[5]);
#ifdef __x86_64__
	sscanf(argv [6], "%lu", &tamanio);
#else
	sscanf(argv [6], "%u", &tamanio);
#endif
	ControlTokens * control = ControlTokens::get_instance(argv[1]);

	//"localbroker1.sitio1.aeropuerto1";
	if (tamanio == 0) {
		MutexDistribuido mutex(argv [1], grupo, id);
		LocalBrokerComm broker(argv [2], hostname, service);

		broker.join(argv [4]);

		while (true) {
			// wait for token
			try {
				broker.wait_mutex(NULL);

				// la aplicacion necesita el token?
				// si lo necesita, actualizo la memoria y habilito el semaforo
				// si no, lo devuelvo
				// espero que termine de procesar
				if (control->comparar_token(grupo)) {
					mutex.entregar_token();
					mutex.esperar_token();
				}

				// devuelvo el token al local_broker
				broker.free_mutex(NULL);
			} catch (OSError & error) {
				break;
			}
		}
	} else {
		MemoriaDistribuida memoria(argv [1], grupo, id);
		LocalBrokerComm broker(argv [2], hostname, service);

		broker.join(argv [4]);

		while (true) {
			// wait for token
			try {
				broker.wait_mutex(memoria.memory_pointer());

				// la aplicacion necesita el token?
				// si lo necesita, actualizo la memoria y habilito el semaforo
				// si no, lo devuelvo
				// espero que termine de procesar
				if (control->comparar_token(grupo)) {
					memoria.entregar_token();
					memoria.esperar_token();
				}

				// devuelvo el token al local_broker
				broker.free_mutex(memoria.memory_pointer());
			} catch (OSError & error) {
				break;
			}
		}
	}

}
