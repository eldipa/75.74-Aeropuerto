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
#include "control_tokens.h"
#include "memoria_distribuida.h"
#include "mutex_distribuido.h"
#include "commerror.h"
#include <sys/wait.h>
#include <unistd.h>
#include "daemon.h"

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
	try {
		socket_broker.sendsome(&mensaje, sizeof(mensajes::mensajes_local_broker_t));

		socket_broker.receivesome(&mensaje, sizeof(mensajes::mensajes_local_broker_t));
	} catch (OSError & error) {
		throw CommError("%s", error.what());
	}
	if (mensaje.respuesta == mensajes::ERROR) {
		throw CommError("Register Error: %s", mensaje.datos);
	}

}

LocalBrokerComm::~LocalBrokerComm() {
	try {
		socket_broker.disassociate();
	} catch (OSError &) {
	}
}

void LocalBrokerComm::join(const std::string & nombre_recurso, mensajes::peticiones_t tipo_join) {
	mensajes::mensajes_local_broker_t mensaje;
	if (tipo_join == mensajes::JOIN_FORK) {
		mensaje.peticion = tipo_join;
	} else {
		mensaje.peticion = mensajes::JOIN;
	}
	strncpy(mensaje.datos, nombre_recurso.c_str(), std::min(nombre_recurso.size() + 1, size_t(MAX_NOMBRE_RECURSO)));

	try {
		socket_broker.sendsome(&mensaje, sizeof(mensajes::mensajes_local_broker_t));

		socket_broker.receivesome(&mensaje, sizeof(mensajes::mensajes_local_broker_t));
	} catch (OSError & error) {
		throw CommError("%s", error.what());
	}
	if (mensaje.respuesta == mensajes::ERROR) {
		throw CommError("Join Error: %s", mensaje.datos);
	} else {
#ifdef __x86_64__
		sscanf(mensaje.datos, "%lu:%lu", &this->cantidad_de_bloques, &this->tamanio_memoria);
#else
		sscanf(mensaje.datos, "%u:%u", &this->cantidad_de_bloques, &this->tamanio_memoria);
#endif
	}
}

void LocalBrokerComm::leave() {
	try {
		socket_broker.disassociate();
	} catch (OSError &) {
	}
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
				throw CommError("Conexión con Broker Rota");
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
		try {
			socket_broker.sendsome(&mensaje, sizeof(mensajes::mensajes_local_broker_token_t));
		} catch (OSError & error) {
			throw CommError("%s", error.what());
		}
	}
}

size_t LocalBrokerComm::get_mem_size() {
	return this->tamanio_memoria;
}

void obtener_lista_de_brokers(std::vector<std::string> & brokers, std::vector<std::string> & servicios, char * archivo)
{
	FILE * f;
	char hostname [MAX_NOMBRE_RECURSO];
	char service [10];
	f = fopen(archivo, "rt");

	if (f == NULL) {
		throw OSError("No se pude obtener la lista de brokers: %s", archivo);
	}
	// evito la primera linea
	fscanf(f, "%*[^\n]\n"); // Evito la primera linea

	while (fscanf(f, "%[^:]:%[^\n]\n", hostname, service) != EOF) {
		brokers.push_back(std::string(hostname));
		servicios.push_back(std::string(service));
	}

	fclose(f);
}

void print_ints(int *p, int cant) {
	std::cout << p [0];
	for (int i = 0 ; i < cant ; i++) {
		std::cout << " " << p [i];
	}
	std::cout << std::endl;
}

LocalBrokerComm * conectar_con_broker(const std::string & nombre_app, std::vector<std::string> & brokers,
	std::vector<std::string> & servicios, size_t & conectado_con)
{

	LocalBrokerComm * broker;
	bool conecto = false;
	size_t i = 0;
	conectado_con = 0;

	do {

		try {
			std::cout << "conectando: " << brokers [i] << ":" << servicios [i] << std::endl;
			broker = new LocalBrokerComm(nombre_app, brokers [i], servicios [i]);
			conecto = true;
		} catch (CommError & error) {
			//delete broker;
			i++;
			if (i == brokers.size()) {
				throw error;
			}
			conecto = false;

		} catch (OSError & error) {
			//delete broker;
			i++;
			if (i == brokers.size()) {
				throw error;
			}
			conecto = false;

		}
		conectado_con++;
	} while (i < brokers.size() && !conecto);

	if (!conecto) {
		throw CommError("Unable to connect to brokers");
	}
	return broker;
}

void do_loop_mutex(ControlTokens * control, const std::string & directorio_de_trabajo, const std::string & nombre_app,
	const std::string & nombre_grupo, char id, std::vector<std::string> & brokers, std::vector<std::string> & servicios)
{

	MutexDistribuido mutex(directorio_de_trabajo, nombre_grupo, id);

	LocalBrokerComm * broker = NULL;
	bool seguir;
	bool reconectar;
	size_t conectado_con = 0;
	seguir = true;
	reconectar = true;
	pid_t pid_hijo;

	while (reconectar) {
		try {
			//std::cout << getpid() << "Padre reconectando: " << std::endl;
			broker = conectar_con_broker(nombre_app, brokers, servicios, conectado_con);
			broker->join(nombre_grupo, mensajes::JOIN_FORK);
			seguir = true;
		} catch (CommError & error) {
			seguir = false;
		}
		reconectar = false;
		ignore_signals();
		pid_hijo = fork();

		if (pid_hijo) { // padre recibe token
			ignore_childs();
			while (seguir) {
				try {
					// wait for token
					//std::cout << getpid() << "Padre esperando token socket" << std::endl;
					broker->wait_mutex(NULL);

					// la aplicacion necesita el token?
					// si lo necesita, actualizo la memoria y habilito el semaforo
					// si no, lo devuelvo
					// espero que termine de procesar
					//std::cout << getpid() << "Padre entregando token semaforo" << std::endl;
					if (control->comparar_token(nombre_grupo.c_str())) {
						mutex.entregar_token();
						//mutex.esperar_token();
					} else {
						// devuelvo el token al local_broker

						mutex.forwardear_token();
					}

				} catch (CommError & error) {
					int status = -1;
					reconectar = true;
					seguir = false;
					//std::cout << getpid() << "Padre matando hijo" << std::endl;
					kill(pid_hijo, SIGTERM);
					//std::cout << getpid() << "Padre esperando fin hijo" << std::endl;
					waitpid(pid_hijo, &status, 0);
					break;
				} catch (OSError & error) {
					//std::cout << getpid() << "Padre oserror finalizando" << std::endl;
					reconectar = false;
					seguir = false;
					//broker->leave();
					//std::cout << "padre leave" << std::endl;
					break;
				}
			}
		} else {

			while (seguir) { // hijo envia token
				try {
					try {
						//std::cout << getpid() << "Hijo esperando token semaforo" << std::endl;
						mutex.esperar_token();
						//std::cout << getpid() << "Hijo enviando token socket" << std::endl;
						broker->free_mutex(NULL);
					} catch (OSError & error) {
						//broker->leave();
						reconectar = false;
						seguir = false;
						//std::cout << getpid() << "Hijo oserror: saliendo" << std::endl;
						//kill(getppid(), SIGTERM);

						//std::cout << "hijo leave" << std::endl;
						break;
					}

				} catch (CommError & error) {
					reconectar = false;
					seguir = false;
					//std::cout << getpid() << "Hijo commerror: saliendo" << std::endl;
					break;

				}
			}
		}
	}
	if (broker)
		delete broker;
}

void do_loop_memoria(ControlTokens * control, const std::string & directorio_de_trabajo, const std::string & nombre_app,
	const std::string & nombre_grupo, char id, std::vector<std::string> & brokers, std::vector<std::string> & servicios)
{
	LocalBrokerComm * broker = NULL;

	MemoriaDistribuida memoria(directorio_de_trabajo, nombre_grupo, id);

	bool seguir;
	bool reconectar;
	size_t conectado_con = 0;
	seguir = true;
	reconectar = true;
	while (reconectar) {
		try {
			broker = conectar_con_broker(nombre_app, brokers, servicios, conectado_con);
			//std::cout << "Conectado" << std::endl;
			reconectar = false;
			seguir = true;
		} catch (CommError & error) {
			reconectar = false;
			seguir = false;
		}

		broker->join(nombre_grupo, mensajes::JOIN);

		while (seguir) {
			// wait for token
			try {
				//std::cout << "Esperando mutex socket" << std::endl;
				broker->wait_mutex(memoria.memory_pointer());
				/*if (strcmp(grupo,"cinta_escaner_control")==0) {
				 print_ints((int *)memoria.memory_pointer(), int(broker->get_mem_size()/ 4));
				 }*/
				// la aplicacion necesita el token?
				// si lo necesita, actualizo la memoria y habilito el semaforo
				// si no, lo devuelvo
				// espero que termine de procesar
				//std::cout << "Chequeando Aplicacion" << std::endl;
				if (control->comparar_token(nombre_grupo.c_str())) {
					//	std::cout << "Entregando Token Aplicacion" << std::endl;
					memoria.entregar_token();
					//	std::cout << "Esperando Token Aplicacion" << std::endl;
					memoria.esperar_token();
				}

				// devuelvo el token al local_broker
				//std::cout << "Devolviendo Token socket" << std::endl;
				broker->free_mutex(memoria.memory_pointer());
			} catch (CommError & error) {
				reconectar = true;
				if (broker)
					delete broker;
				seguir = false;
				break;
			} catch (OSError & error) {
				//broker->leave();
				reconectar = false;
				break;
			}
		}
	}
}

void test_local_broker_comm() {

	LocalBrokerComm broker("debug", "localbroker1.sitio1.aeropuerto1", "1234");

	broker.join("cinta_principal", mensajes::JOIN);
}

void test_local_broker_comm2(int argc, char * argv []) {
	char *mem = new char [10 * 1024];

	if (argc < 3) {
		std::cerr << "Faltan argumentos (nombre_app,grupo)" << std::endl;
		return;
	}

	LocalBrokerComm broker(argv [1], "localbroker1.sitio1.aeropuerto1", "1234");

	broker.join(argv [2], mensajes::JOIN);

	for (int i = 0 ; i < 3 ; i++) {

		broker.wait_mutex(mem);

		broker.free_mutex(mem);
	}
	delete mem;
}

int main(int argc, char * argv [])
try
{
	char grupo [MAX_NOMBRE_RECURSO];
	size_t tamanio;
	char id;
	std::string hostname;
	std::string service;
	std::vector<std::string> brokers;
	std::vector<std::string> servicios;

	//test_local_broker_comm();
	test_local_broker_comm2(argc, argv);

	// maneja la comunicacion con el broker del lado del cliente
	if (argc < 4) {
		std::cerr << "Faltan argumentos" << std::endl;
		std::cerr << "1: directorio de trabajo" << std::endl;
		std::cerr << "2: nombre de la aplicacion" << std::endl;
		std::cerr << "3: archivo de lista de brokers locales (HOSTNAME:PUERTO)" << std::endl;
		std::cerr << "4: nombre del grupo" << std::endl;
		std::cerr << "5: id" << std::endl;
		std::cerr << "6: tamanio mem compartida" << std::endl;
		return -1;
	}

	/*std::cout << argv [0];
	 for (int i = 1 ; i < argc ; i++) {
	 std::cout << " " << argv [i];
	 }
	 std::cout << std::endl;*/

	obtener_lista_de_brokers(brokers, servicios, argv [3]);

	strncpy(grupo, argv [4], MAX_NOMBRE_RECURSO);
	id = atoi(argv [5]);
#ifdef __x86_64__
	sscanf(argv [6], "%lu", &tamanio);
#else
	sscanf(argv [6], "%u", &tamanio);
#endif

	ControlTokens * control = ControlTokens::get_instance(argv [1]);

	//"localbroker1.sitio1.aeropuerto1";
	if (tamanio == 0) {
		try {
			do_loop_mutex(control, argv [1], argv [2], argv [4], id, brokers, servicios);
		} catch (OSError & error) {
		}
	} else {
		try {
			do_loop_memoria(control, argv [1], argv [2], argv [4], id, brokers, servicios);
		} catch (OSError & error) {
		}
	}
	control->destroy_instance();
}
catch (CommError & e) {
	std::cerr << e.what() << std::endl;
}
catch (OSError & e) {
	//std::cerr << e.what() << std::endl;
	//std::cerr << "Unable to connect to local_brokers" << std::endl;
}
catch (std::exception & e) {
	std::cerr << e.what() << std::endl;
}

