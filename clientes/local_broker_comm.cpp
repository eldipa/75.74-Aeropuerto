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
#include "semaphore_set_distribuido.h"
#include "commerror.h"
#include <sys/wait.h>
#include <unistd.h>
#include "daemon.h"
#include "log.h"
#include "signalhandler.h"
#include "eventhandler.h"

#define DEBUG_LOCAL_BROKER_COMM 0

sig_atomic_t LocalBrokerComm::salir = 0;

class UnHandler: public EventHandler {
private:
	sig_atomic_t * salir;
public:
	UnHandler(sig_atomic_t * salir) {
		this->salir = salir;
		SignalHandler::getInstance()->registrarHandler(SIGUSR1, this);
		SignalHandler::getInstance()->registrarHandler(SIGUSR2, this);
	}
	virtual ~UnHandler() {

	}
	virtual void handleSignal(int signum) {
		if (signum == SIGUSR1) {
			*salir = 1;
		} else if (signum == SIGUSR2) {
			*salir = 2;
		}
	}
};

void print_ints(int *p, int cant) {
	std::cout << p [0];
	for (int i = 1 ; i < cant ; i++) {
		std::cout << " " << p [i];
	}
	std::cout << std::endl;
}

LocalBrokerComm::LocalBrokerComm(const std::string & app_name, const std::string & broker_hostname,
	const std::string & service)
	: socket_broker(true), nombre_aplicacion(app_name)
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
	tengo_token = false;

}

LocalBrokerComm::~LocalBrokerComm() {
	try {
		leave();
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
	nombre_grupo = nombre_recurso;
}

void LocalBrokerComm::leave() {
	mensajes::mensajes_local_broker_token_t mensaje;
	try {
#if DEBUG_LOCAL_BROKER_COMM == 1
		std::cout << nombre_aplicacion << " " << nombre_grupo << " Enviando LEAVED" << std::endl;
#endif
		mensaje.peticion = mensajes::LEAVED;
		socket_broker.sendsome((char *)&mensaje, sizeof(mensajes::mensajes_local_broker_token_t));
#if DEBUG_LOCAL_BROKER_COMM == 1
		std::cout << nombre_aplicacion << " " << nombre_grupo << " Waiting LEAVED Reply" << std::endl;
#endif
		socket_broker.receivesome((char *)&mensaje, sizeof(mensajes::mensajes_local_broker_token_t));
#if DEBUG_LOCAL_BROKER_COMM == 1
		std::cout << nombre_aplicacion << " " << nombre_grupo << " Finishing" << std::endl;
#endif
		socket_broker.disassociate();
	} catch (OSError &) {
	}
}

void LocalBrokerComm::wait_mutex(void * memory) {
	ssize_t leidos;
	mensajes::mensajes_local_broker_token_t mensaje;
	size_t cant = 0;
	size_t i;

	for (i = 0; i < this->cantidad_de_bloques && salir == 0 ; i++) {
		cant = 0;
		do {
			leidos = socket_broker.receivesome((char *)&mensaje + cant,
				sizeof(mensajes::mensajes_local_broker_token_t) - cant);

			if (leidos == 0) {
				//return 0;
				throw CommError("Conexión con Broker Rota");
			}
			if (leidos != -1) {
				cant += leidos;
				tengo_token = true;
			}
		} while (cant < sizeof(mensajes::mensajes_local_broker_token_t));
		if (tamanio_memoria != 0 && memory != NULL && salir == 0) {
			memcpy((char *)memory + i * DATA_SIZE, mensaje.datos,
				std::min(tamanio_memoria - i * DATA_SIZE, size_t(DATA_SIZE)));
#if DEBUG_LOCAL_BROKER_COMM == 1
			if (i == 0 && tamanio_memoria != 0) {

				std::cout << nombre_aplicacion << " Recibido: ";
				print_ints((int *)memory, 9);
			}
#endif
		}
	}

}

void LocalBrokerComm::free_mutex(void * memory) {
	//mensajes::mensajes_local_broker_token_t mensaje;
	//memcpy(mensaje.datos, memory, this->cantidad_de_bloques);
	//socket_broker.sendsome(&mensaje, sizeof(mensajes::mensajes_local_broker_token_t));
	size_t enviados;
	mensajes::mensajes_local_broker_token_t mensaje;
	size_t i;
	mensaje.peticion = mensajes::TOKEN_FREE;
	for (i = 0; i < this->cantidad_de_bloques ; i++) {
		if (tamanio_memoria != 0 && memory != NULL) {
			memcpy(mensaje.datos, ((char *)memory + i * DATA_SIZE),
				std::min(this->tamanio_memoria - i * DATA_SIZE, size_t(DATA_SIZE)));
		}
		try {
			enviados = 0;
			while (enviados < sizeof(mensajes::mensajes_local_broker_token_t)) {
				mensaje.peticion = (salir == 0) ? mensajes::TOKEN_FREE : mensajes::TOKEN_LEAVE;
				enviados += socket_broker.sendsome((char *)&mensaje + enviados,
					sizeof(mensajes::mensajes_local_broker_token_t) - enviados);
			}
			tengo_token = false;
		} catch (OSError & error) {
			throw CommError("%s", error.what());
		}
#if DEBUG_LOCAL_BROKER_COMM == 1
		if (nombre_grupo == "cpp_sem_cons_0") {
			std::cout << "Enviado token cpp_sem_cons_0" << std::endl;
		}
		if (i == 0 && tamanio_memoria != 0) {
			std::cout << nombre_aplicacion << " Enviado: ";
			print_ints((int *)memory, 9);
		}
#endif
	}
}

size_t LocalBrokerComm::get_mem_size() {
	return this->tamanio_memoria;
}

void LocalBrokerComm::set_salir() {
	salir = 1;
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

void conectar_con_broker(LocalBrokerComm ** broker, const std::string & nombre_app, std::vector<std::string> & brokers,
	std::vector<std::string> & servicios, size_t & conectado_con)
{

	bool conecto = false;
	size_t cant_brokers = brokers.size();
	size_t i = conectado_con;

	if (*broker != NULL) {
		delete *broker;
		i = (i + 1) % cant_brokers;
	}

	do {
		try {
			Log::info("Conectando: %s : %s\n", brokers [i].c_str(), servicios [i].c_str());
			//std::cout << "conectando: " << brokers [i] << ":" << servicios [i] << std::endl;
			*broker = new LocalBrokerComm(nombre_app, brokers [i], servicios [i]);
			conecto = true;
			conectado_con = i;
		} catch (CommError & error) {
			i = (i + 1) % cant_brokers;
			conecto = false;
		} catch (OSError & error) {
			i = (i + 1) % cant_brokers;
			conecto = false;
		}
	} while (!conecto && i != conectado_con && LocalBrokerComm::salir == 0);

	if (!conecto) {
		throw CommError("Unable to connect to brokers");
	}

}

void do_loop_mutex(ControlTokens * control, const std::string & directorio_de_trabajo, const std::string & nombre_app,
	const std::string & nombre_grupo, char id, std::vector<std::string> & brokers, std::vector<std::string> & servicios)
{

	MutexDistribuido mutex(directorio_de_trabajo, nombre_grupo, id);

	LocalBrokerComm * broker = NULL;
	bool seguir = true;
	bool reconectar = true;
	bool tengo_token = false;
	size_t conectado_con = 0;
	pid_t pid_hijo;

	ignore_signals();

	while (reconectar && LocalBrokerComm::salir == 0) {
		try {
			//std::cout << getpid() << "Padre reconectando: " << std::endl;
			conectar_con_broker(&broker, nombre_app, brokers, servicios, conectado_con);
			broker->join(nombre_grupo, mensajes::JOIN_FORK);
			seguir = true;
		} catch (CommError & error) {
			Log::crit("Unable to connect to local brokers");
			throw error;
		}
		reconectar = false;

		pid_hijo = fork();
		if (pid_hijo) { // padre recibe token
			ignore_childs();
			while (seguir && LocalBrokerComm::salir == 0) {
				try {
					// wait for token
					//std::cout << getpid() << "Padre esperando token socket" << std::endl;
					//Log::debug("Esperando token %s (%s)", nombre_grupo.c_str(), nombre_app.c_str());
					broker->wait_mutex(NULL);
					tengo_token = true;
					// la aplicacion necesita el token?
					// si lo necesita, actualizo la memoria y habilito el semaforo
					// si no, lo devuelvo
					// espero que termine de procesar
					//std::cout << getpid() << "Padre entregando token semaforo" << std::endl;
					if (control->comparar_token(nombre_grupo.c_str())) {
						//	Log::debug("Entregando Token Aplicacion %s (%s)", nombre_grupo.c_str(), nombre_app.c_str());
						mutex.entregar_token();
						//mutex.esperar_token();
					} else {
						// devuelvo el token al local_broker
						mutex.forwardear_token();
					}
					tengo_token = false;

				} catch (CommError & error) {
					reconectar = true;
					seguir = false;
					break;
				} catch (OSError & error) {
					reconectar = false;
					seguir = false;
					break;
				}
			}
		} else {

			while (seguir && LocalBrokerComm::salir == 0) { // hijo envia token
				try {
					try {
						mutex.esperar_token();
						Log::debug("Devolviendo token %s (%s)", nombre_grupo.c_str(), nombre_app.c_str());
						broker->free_mutex(NULL);
					} catch (OSError & error) {
						reconectar = false;
						seguir = false;
						break;
					}

				} catch (CommError & error) {
					reconectar = false;
					seguir = false;
					break;

				}
			}
		}
	}
	if (LocalBrokerComm::salir == 1 && pid_hijo != 0) {
		int status = -1;
		if (tengo_token) {
			mutex.forwardear_token();
		}
		kill(pid_hijo, SIGUSR1);
		waitpid(pid_hijo, &status, 0);
	}
	if (broker)
		delete broker;
}

bool LocalBrokerComm::tiene_token() {
	return tengo_token;
}

void LocalBrokerComm::set_tiene_token(bool t) {
	tengo_token = t;
}

void do_loop_semaforo(ControlTokens * control, const std::string & directorio_de_trabajo,
	const std::string & nombre_app, const std::string & nombre_grupo, char id, int num_sem, int cant_sem,
	std::vector<std::string> & brokers, std::vector<std::string> & servicios)
{

	LocalBrokerComm * broker = NULL;
	bool seguir = true;
	bool reconectar = true;
	size_t conectado_con = 0;
	size_t size;
	std::string nombre(nombre_grupo);
	pid_t pid_hijo;
	bool forward;

	size = nombre.size();
	while (nombre [size - 1] >= '0' && nombre [size - 1] <= '9') {
		nombre.erase(size - 1, 1);
		size--;
	}

	SemaphoreSetDistribuido semaforos(directorio_de_trabajo, nombre, id, cant_sem);

	while (reconectar && LocalBrokerComm::salir == 0) {
		try {
			//std::cout << getpid() << "Padre reconectando: " << std::endl;
			conectar_con_broker(&broker, nombre_app, brokers, servicios, conectado_con);
			broker->join(nombre_grupo, mensajes::JOIN_FORK);
			seguir = true;
		} catch (CommError & error) {
			Log::crit("Unable to connect to local brokers");
			throw error;
		}
		reconectar = false;
		ignore_signals();
		pid_hijo = fork();

		if (pid_hijo) { // padre recibe token
			ignore_childs();
			while (seguir && LocalBrokerComm::salir == 0) {
				try {

					broker->wait_mutex(NULL);

					if (broker->tiene_token() && LocalBrokerComm::salir == 0) {
						try {
							forward = !control->comparar_token(nombre_grupo.c_str());
						} catch (OSError) {
							forward = true;
						}
						if (!forward) {
							semaforos.entregar_token(num_sem);
						} else {
							semaforos.forwardear_token(num_sem);
						}
						broker->set_tiene_token(false);
					} else if(broker->tiene_token() && LocalBrokerComm::salir == 1){
#if DEBUG_LOCAL_BROKER_COMM == 1
						std::cout << "Forward token " << nombre_grupo << std::endl;
#endif
						semaforos.forwardear_token(num_sem);
					}
				} catch (CommError & error) {
					reconectar = true;
					seguir = false;
					break;
				} catch (OSError & error) {
					reconectar = false;
					seguir = false;
					break;
				}
			}
		} else {

			while (seguir && LocalBrokerComm::salir == 0) { // hijo envia token
				try {
					try {
						semaforos.esperar_token(num_sem);
						broker->free_mutex(NULL);
					} catch (OSError & error) {
						reconectar = false;
						seguir = false;
						break;
					}
				} catch (CommError & error) {
					reconectar = false;
					seguir = false;
					break;
				}
			}
		}
	}
	if (LocalBrokerComm::salir == 1 && pid_hijo != 0) {
		int status = -1;
		if (broker->tiene_token()) {
			kill(pid_hijo, SIGUSR2);
		} else {
			kill(pid_hijo, SIGUSR1);
		}
		waitpid(pid_hijo, &status, 0);
	} else if (LocalBrokerComm::salir == 2 && pid_hijo == 0) {
		broker->free_mutex(NULL);
	}
	if (broker)
		delete broker;
}

void do_loop_memoria(ControlTokens * control, const std::string & directorio_de_trabajo, const std::string & nombre_app,
	const std::string & nombre_grupo, char id, std::vector<std::string> & brokers, std::vector<std::string> & servicios)
{
	LocalBrokerComm * broker = NULL;
	bool seguir = true;
	bool reconectar = true;
	size_t conectado_con = 0;
	void * memory;

	MemoriaDistribuida memoria(directorio_de_trabajo, nombre_grupo, id);
	memory = memoria.memory_pointer();

	while (reconectar && LocalBrokerComm::salir == 0) {
		try {
			conectar_con_broker(&broker, nombre_app, brokers, servicios, conectado_con);
			//std::cout << "Conectado" << std::endl;
			reconectar = false;
			seguir = true;
		} catch (CommError & error) {
			Log::crit("Unable to connect to local brokers");
			throw error;
		}

		reconectar = false;
		broker->join(nombre_grupo, mensajes::JOIN);

		while (seguir && LocalBrokerComm::salir == 0) {
			// wait for token
			try {
				//Log::debug("Esperando token %s (%s)", nombre_grupo.c_str(), nombre_app.c_str());
				broker->wait_mutex(memory);
				/*if (strcmp(grupo,"cinta_escaner_control")==0) {
				 print_ints((int *)memoria.memory_pointer(), int(broker->get_mem_size()/ 4));
				 }*/
				// la aplicacion necesita el token?
				// si lo necesita, actualizo la memoria y habilito el semaforo
				// si no, lo devuelvo
				// espero que termine de procesar
				//std::cout << "Chequeando Aplicacion" << std::endl;
				if (control->comparar_token(nombre_grupo.c_str())) {
					//	Log::debug("Entregando Token Aplicacion %s (%s)", nombre_grupo.c_str(), nombre_app.c_str());
					//	std::cout << "Entregando Token Aplicacion" << std::endl;
					memoria.entregar_token();
					//	std::cout << "Esperando Token Aplicacion" << std::endl;
					memoria.esperar_token();
				}

				// devuelvo el token al local_broker
				//std::cout << "Devolviendo Token socket" << std::endl;
				//Log::debug("Devolviendo Token %s (%s)", nombre_grupo.c_str(), nombre_app.c_str());
				broker->free_mutex(memory);
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
	if (broker) {
		delete broker;
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

void print_args(int argc, char * argv []) {
	int i;
	std::cout << "argc=" << argc << std::endl;
	std::cout << argv [0];
	for (i = 1; i < argc ; i++) {
		std::cout << " " << argv [i];
	}
	std::cout << std::endl;
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

	//print_args(argc,argv);

	chdir("processes");

	//test_local_broker_comm();
	//test_local_broker_comm2(argc, argv);

	// maneja la comunicacion con el broker del lado del cliente
	if (argc < 7) {
		std::cerr << "Faltan argumentos" << std::endl;
		std::cerr << "1: directorio de trabajo" << std::endl;
		std::cerr << "2: nombre de la aplicacion" << std::endl;
		std::cerr << "3: archivo de lista de brokers locales (HOSTNAME:PUERTO)" << std::endl;
		std::cerr << "4: nombre del grupo" << std::endl;
		std::cerr << "5: id" << std::endl;
		std::cerr << "6: tamanio mem compartida" << std::endl;
		std::cerr << "7: tipo (sem/mutex)" << std::endl;
		std::cerr << "8: numero semaforo" << std::endl;
		std::cerr << "9: cantidad de semaforos" << std::endl;
		return -1;
	}

	obtener_lista_de_brokers(brokers, servicios, argv [3]);

	strncpy(grupo, argv [4], MAX_NOMBRE_RECURSO);
	id = atoi(argv [5]);

#ifdef __x86_64__
	sscanf(argv [6], "%lu", &tamanio);
#else
	sscanf(argv [6], "%u", &tamanio);
#endif

	UnHandler signalhandler(&LocalBrokerComm::salir);

	bool seguir = true;
	int i = 0;
	ControlTokens * control;
	do {
		try {
			control = ControlTokens::get_instance(argv [1]);
			seguir = false;
		} catch (OSError &) {
			i++;
		}
	} while (seguir && i < 5 && LocalBrokerComm::salir == 0);

	if (control != NULL && LocalBrokerComm::salir == 0) {
		try {
			if (tamanio == 0) {
				if (strncmp(argv [7], "mutex", strlen("mutex")) == 0) {
					do_loop_mutex(control, argv [1], argv [2], argv [4], id, brokers, servicios);
				} else if (strncmp(argv [7], "sem", strlen("sem")) == 0) {
					do_loop_semaforo(control, argv [1], argv [2], argv [4], id, atoi(argv [8]), atoi(argv [9]), brokers,
						servicios);
				}
			} else {
				do_loop_memoria(control, argv [1], argv [2], argv [4], id, brokers, servicios);
			}
		} catch (OSError & error) {
			Log::crit(error.what());
		}
		control->destroy_instance();
	}
	sleep(2);
}
catch (CommError & e) {
	//std::cerr << e.what() << std::endl;
	Log::crit(e.what());
}
catch (OSError & e) {
	//std::cerr << e.what() << std::endl;
	//std::cerr << "Unable to connect to local_brokers" << std::endl;
	Log::crit(e.what());
}
catch (std::exception & e) {
	//std::cerr << e.what() << std::endl;
	Log::crit(e.what());
}

