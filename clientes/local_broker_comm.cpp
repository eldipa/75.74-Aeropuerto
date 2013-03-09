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
#include <sys/wait.h>
#include <unistd.h>

#include "genericerror.h"
#include "control_tokens.h"
#include "commerror.h"
#include "interrupted.h"
#include "daemon.h"
#include "log.h"
#include "debug_flags.h"
#include <sstream>

void print_ints(int *p, int cant) {
	std::cout << p [0];
	for (int i = 1 ; i < cant ; i++) {
		std::cout << " " << p [i];
	}
	std::cout << std::endl;
}

LocalBrokerComm::LocalBrokerComm(const std::string & directorio_de_trabajo, const std::string & app_name,
	const std::string & group_name, const std::vector<std::string> & broker_hostnames,
	const std::vector<std::string> & services, tipo_ipc_t tipo, int num_sem, int cantidad_de_semaforos)
	: nombre_aplicacion(app_name), nombre_grupo(group_name), numero_de_semaforo(num_sem), tipo_de_recurso(tipo),
		lista_de_brokers(broker_hostnames), lista_de_servicios(services)
{
	size_t size;
	conectado = false;

	control = ControlTokens::get_instance(directorio_de_trabajo);

	tengo_token = false;

	if (tipo_de_recurso == SHAREDMEMORY) {
		memoria = new MemoriaDistribuida(directorio_de_trabajo, nombre_grupo, char(0));
	} else if (tipo_de_recurso == MUTEX) {
		mutex = new MutexDistribuido(directorio_de_trabajo, nombre_grupo, char(0));
	} else if (tipo_de_recurso == SEMAFORO) {
		std::string nombre(nombre_grupo);
		size = nombre.size();
		while (nombre [size - 1] >= '0' && nombre [size - 1] <= '9') {
			nombre.erase(size - 1, 1);
			size--;
		}
		semaforo = new SemaphoreSetDistribuido(directorio_de_trabajo, nombre, char(0), cantidad_de_semaforos);
	}

	SignalHandler::getInstance()->registrarHandler(SIGUSR1, this);
	SignalHandler::getInstance()->registrarHandler(SIGUSR2, this);

	salir = 0;

}

LocalBrokerComm::~LocalBrokerComm() {
	ControlTokens::destroy_instance();
	SignalHandler::destruir();
}

void LocalBrokerComm::handleSignal(int signum) {
	if (signum == SIGUSR1) {
		salir = 1;
#if DEBUG_LOCAL_BROKER_COMM == 1
		std::cout << nombre_aplicacion << "(" << nombre_grupo << "-" << getpid() << "): señal recibida salir = 1"
			<< std::endl;
#endif
	} else if (signum == SIGUSR2) {
		salir = 2;
#if DEBUG_LOCAL_BROKER_COMM == 1
		std::cout << nombre_aplicacion << "(" << nombre_grupo << "-" << getpid() << "): señal recibida salir = 2"
			<< std::endl;
#endif
	}
}

void LocalBrokerComm::registrar() {
	mensajes::mensajes_local_broker_t msg;
	msg.peticion = mensajes::REGISTER;
	bzero(msg.datos, sizeof(msg.datos));
	snprintf(msg.datos, sizeof(msg.datos), "%s", nombre_aplicacion.c_str());

	// Hago un pedido de registro
	try {
		socket_broker->sendsome(&msg, sizeof(mensajes::mensajes_local_broker_t));

		socket_broker->receivesome(&msg, sizeof(mensajes::mensajes_local_broker_t));
	} catch (OSError & error) {
		throw CommError("%s", error.what());
	}
	if (msg.respuesta == mensajes::ERROR) {
		throw CommError("Register Error: %s", msg.datos);
	}
}

void LocalBrokerComm::join() {
	mensajes::mensajes_local_broker_t msg;

	if (tipo_de_recurso == SHAREDMEMORY) {
		msg.peticion = mensajes::JOIN;
	} else {
		msg.peticion = mensajes::JOIN_FORK;
	}
	strncpy(msg.datos, nombre_grupo.c_str(), std::min(nombre_grupo.size() + 1, size_t(MAX_NOMBRE_RECURSO)));

	try {
		socket_broker->sendsome(&msg, sizeof(mensajes::mensajes_local_broker_t));

		socket_broker->receivesome(&msg, sizeof(mensajes::mensajes_local_broker_t));
	} catch (OSError & error) {
		throw CommError("%s", error.what());
	}
	if (msg.respuesta == mensajes::ERROR) {
		throw CommError("Join Error: %s", msg.datos);
	} else {
#ifdef __x86_64__
		sscanf(msg.datos, "%lu:%lu", &this->cantidad_de_bloques, &this->tamanio_memoria);
#else
		sscanf(msg.datos, "%u:%u", &this->cantidad_de_bloques, &this->tamanio_memoria);
#endif
	}
}

void LocalBrokerComm::reconectar() {
	bool conecto = false;
	size_t i = 0;
	size_t cant_brokers = std::min(lista_de_brokers.size(), lista_de_servicios.size());

	if (!conectado) { // primera vez que me conecto
		do {
			try {
				Log::debug("Conectando con %s : %s\n", lista_de_brokers [i].c_str(), lista_de_servicios [i].c_str());
				//std::cout << "conectando: " << brokers [i] << ":" << servicios [i] << std::endl;
				socket_broker = new Socket(true);
				socket_broker->destination(lista_de_brokers [i].c_str(), lista_de_servicios [i].c_str());
				conecto = true;
				conectado = true;
				numero_broker_conectado = i;
			} catch (OSError & error) {
				i = (i + 1) % cant_brokers;
				if (i == 0) {
					throw CommError("Unable to connect to local brokers\n");
				}
			}
		} while (!conecto);
	} else {
		conecto = false;
		// reintento conectarme al mismo broker
		for (i = 0; i < 3 ; i++) {
			try {
				socket_broker = new Socket(true);
				Log::debug("Reconectando con %s intento %d", lista_de_brokers [numero_broker_conectado].c_str(), i + 1);
				socket_broker->destination(lista_de_brokers [numero_broker_conectado].c_str(),
					lista_de_servicios [numero_broker_conectado].c_str());
				conecto = true;
				break;
			} catch (OSError & error) {
			}
		}
		while (!conecto) { // trato de conectarme a otro broker
			i = (numero_broker_conectado + 1) % cant_brokers;
			try {
				socket_broker = new Socket(true);
				Log::debug("Conectando con %s : %s\n", lista_de_brokers [i].c_str(), lista_de_servicios [i].c_str());
				socket_broker->destination(lista_de_brokers [i].c_str(), lista_de_servicios [i].c_str());
				conecto = true;
				numero_broker_conectado = i;
				break;
			} catch (OSError & error) {
				i = (i + 1) % cant_brokers;
				if (i == numero_broker_conectado) {
					throw CommError("Unable to connect to local brokers\n");
				}
			}
		}
	}

	// Registro la aplicacion
	registrar();

	// hago join con el recurso
	join();

}

void LocalBrokerComm::leave() {
	mensajes::mensajes_local_broker_token_t mensaje;
	mensaje.peticion = mensajes::LEAVE;
	try {
		do {
#if DEBUG_LOCAL_BROKER_COMM == 1
			std::cout << nombre_aplicacion << "(" << nombre_grupo << ") Enviando Leave " << "" << std::endl;
#endif
			socket_broker->sendsome((char *)&mensaje, sizeof(mensajes::mensajes_local_broker_token_t));
#if DEBUG_LOCAL_BROKER_COMM == 1
			std::cout << nombre_aplicacion << "(" << nombre_grupo << ") Esperando respuesta Leave" << std::endl;
#endif
			if (tipo_de_recurso != SHAREDMEMORY) {
				socket_broker->receivesome((char *)&mensaje, sizeof(mensajes::mensajes_local_broker_token_t));
				if (mensaje.respuesta == mensajes::R_TOKEN) {
#if DEBUG_LOCAL_BROKER_COMM == 1
					std::cout << nombre_aplicacion << "(" << nombre_grupo << ") Recibido Token" << std::endl;
#endif
					mensaje.peticion = mensajes::TOKEN_LEAVE;
				} else {
#if DEBUG_LOCAL_BROKER_COMM == 1
					std::cout << nombre_aplicacion << "(" << nombre_grupo << ") Recibido R_TOKEN_CHECK" << std::endl;
#endif
					mensaje.peticion = mensajes::LEAVE;
				}
			} else {
				mensaje.respuesta = mensajes::LEAVE_OK;
			}
		} while (mensaje.respuesta != mensajes::LEAVE_OK);
#if DEBUG_LOCAL_BROKER_COMM == 1
		std::cout << nombre_aplicacion << " " << nombre_grupo << " Terminando" << std::endl;
#endif
		socket_broker->disassociate();
	} catch (OSError &) {
	}
}

void LocalBrokerComm::wait_token() {
	ssize_t leidos;

	tengo_token = 0;
	try {
		leidos = socket_broker->receivesome((char *)&mensaje, sizeof(mensajes::mensajes_local_broker_token_t));
		tengo_token = 1;

		if (leidos == 0) {
			throw CommError("El broker cerro la conexion\n");
		}
	} catch (InterruptedSyscall & interruption) {
#if DEBUG_LOCAL_BROKER_COMM == 1
		std::cout << nombre_aplicacion << "(" << nombre_grupo << "): Interrupted receive" << std::endl;
		//std::cout << nombre_aplicacion << "(" << nombre_grupo << "): " << interruption.what() << std::endl;
#endif
		Log::alert(interruption.what());
		tengo_token = 0;
	}

#if DEBUG_LOCAL_BROKER_COMM == 1
	std::cout << nombre_aplicacion << "(" << nombre_grupo << ") tengo token: " << std::endl;
#endif
}

void LocalBrokerComm::send_token() {
	bool reintentar = true;
	mensaje.peticion = mensajes::TOKEN_FREE;

	while (reintentar) {
		try {
			socket_broker->sendsome((char *)&mensaje, sizeof(mensajes::mensajes_local_broker_token_t));
			reintentar = false;
		} catch (InterruptedSyscall & interruption) {
			Log::alert("%s(%s) %s", nombre_aplicacion.c_str(), nombre_grupo.c_str(), interruption.what());
			reintentar = true;
		}
	}
#if DEBUG_LOCAL_BROKER_COMM == 1
	std::cout << "Enviado token " << nombre_grupo << std::endl;
#endif
}

void LocalBrokerComm::wait_token(void * memory) {
	ssize_t leidos;
	size_t cant = 0;
	size_t i;

	for (i = 0; i < this->cantidad_de_bloques ; i++) {
		cant = 0;

		do {
			leidos = socket_broker->receivesome((char *)&mensaje + cant,
				sizeof(mensajes::mensajes_local_broker_token_t) - cant);

			if (leidos == 0) {
				throw CommError("El broker cerro la conexion\n");
			}
			cant += leidos;
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
	tengo_token = 1;
#if DEBUG_LOCAL_BROKER_COMM == 1
	std::cout << nombre_aplicacion << "(" << nombre_grupo << ") tengo token" << std::endl;
#endif
}

void LocalBrokerComm::send_token(void * memory) {
	//mensajes::mensajes_local_broker_token_t mensaje;
	//memcpy(mensaje.datos, memory, this->cantidad_de_bloques);
	//socket_broker.sendsome(&mensaje, sizeof(mensajes::mensajes_local_broker_token_t));
	size_t enviados;
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
				try {
					enviados += socket_broker->sendsome((char *)&mensaje + enviados,
						sizeof(mensajes::mensajes_local_broker_token_t) - enviados);
				} catch (InterruptedSyscall & interruption) {
					Log::alert(interruption.what());
				}
			}
			tengo_token = false;
		} catch (OSError & error) {
			throw CommError("%s", error.what());
		}
#if DEBUG_LOCAL_BROKER_COMM == 1
		if (i == 0 && tamanio_memoria != 0) {
			std::cout << nombre_aplicacion << " Enviado: ";
			print_ints((int *)memory, 9);
		}
#endif
	}
}

void LocalBrokerComm::loop_mutex_padre() {
	while (salir == 0) {
		try {
			wait_token();
			if (tengo_token == 1) {
				if (control->comparar_token(nombre_grupo.c_str())) {
					mutex->entregar_token();
				} else {
					// devuelvo el token al local_broker
#if DEBUG_LOCAL_BROKER_COMM == 1
					std::cout << "Forward token " << nombre_grupo << std::endl;
#endif
					mutex->forwardear_token();
					tengo_token = 0;
				}
			}
		} catch (CommError & error) {
#if DEBUG_LOCAL_BROKER_COMM == 1
			std::cout << "CommError " << nombre_grupo << std::endl;
			std::cout << error.what() << std::endl;
#endif
			salir = 1;
			Log::crit(error.what());
			break;
		} catch (OSError & error) {
#if DEBUG_LOCAL_BROKER_COMM == 1
			std::cout << "OSerror " << nombre_grupo << std::endl;
#endif
			Log::crit(error.what());
			salir = 1;
			break;
		}
	}
}

void LocalBrokerComm::loop_mutex_hijo() {

	while (salir == 0) { // hijo envia token
		try {

			mutex->esperar_token();

			send_token();

		} catch (CommError & error) {
			Log::crit(error.what());
			break;

		} catch (OSError & error) {
			Log::crit(error.what());
			break;
		}
	}

	leave();
}

void LocalBrokerComm::loop_mutex() {
	pid_t pid_hijo;

	ignore_signals();

	while (salir == 0) {
		reconectar();
		ignore_signals();
		pid_hijo = fork();

		if (pid_hijo) { // padre recibe token
			ignore_childs();
			loop_mutex_padre();
			int status = -1;
			kill(pid_hijo, SIGUSR1);
#if DEBUG_LOCAL_BROKER_COMM == 1
			std::cout << nombre_aplicacion << "(" << nombre_grupo << "): esperando hijo" << std::endl;
#endif
			waitpid(pid_hijo, &status, 0);
#if DEBUG_LOCAL_BROKER_COMM == 1
			std::cout << nombre_aplicacion << "(" << nombre_grupo << "): hijo termino" << std::endl;
#endif
		} else {
			loop_mutex_hijo();
		}
	}
}

void LocalBrokerComm::loop_semaforo_padre() {
	while (salir == 0) {
		try {
			wait_token();
			if (tengo_token == 1) {
				if (control->comparar_token(nombre_grupo.c_str())) {
					semaforo->entregar_token(numero_de_semaforo);
				} else {
					// devuelvo el token al local_broker
#if DEBUG_LOCAL_BROKER_COMM == 1
					std::cout << "Forward token " << nombre_grupo << std::endl;
#endif
					semaforo->forwardear_token(numero_de_semaforo);
					tengo_token = 0;
				}
			}
		} catch (CommError & error) {
#if DEBUG_LOCAL_BROKER_COMM == 1
			std::cout << "CommError " << nombre_grupo << std::endl;
			std::cout << error.what() << std::endl;
#endif
			salir = 1;
			Log::crit(error.what());
			break;
		} catch (OSError & error) {
#if DEBUG_LOCAL_BROKER_COMM == 1
			std::cout << "OSerror " << nombre_grupo << std::endl;
#endif
			Log::crit(error.what());
			salir = 1;
			break;
		}
	}
}

void LocalBrokerComm::loop_semaforo_hijo() {
	while (salir == 0) { // hijo envia token
		try {

			semaforo->esperar_token(numero_de_semaforo);

			send_token();

		} catch (OSError & error) {
			Log::alert("%s(%s): Broken Pipe\n", nombre_aplicacion.c_str(), nombre_grupo.c_str());
			//Log::alert(error.what());
			break;
		}
	}

	// hijo envia FIN
	leave();
}

void LocalBrokerComm::loop_semaforo() {
	pid_t pid_hijo;

	while (salir == 0) {
		reconectar();
		ignore_signals();
		pid_hijo = fork();

		if (pid_hijo) { // padre recibe token
			ignore_childs();
			loop_semaforo_padre();
			int status = -1;
			kill(pid_hijo, SIGUSR1);
#if DEBUG_LOCAL_BROKER_COMM == 1
			std::cout << nombre_aplicacion << "(" << nombre_grupo << "): esperando hijo" << std::endl;
#endif
			waitpid(pid_hijo, &status, 0);
#if DEBUG_LOCAL_BROKER_COMM == 1
			std::cout << nombre_aplicacion << "(" << nombre_grupo << "): hijo termino" << std::endl;
#endif
		} else {
			loop_semaforo_hijo();
		}
	}
}

void LocalBrokerComm::loop_memoria() {
	void * memory;

	memory = memoria->memory_pointer();

	while (salir == 0) {
		reconectar();

		while (salir == 0) {
			try {
				wait_token(memory);

				if (control->comparar_token(nombre_grupo.c_str())) {
					memoria->entregar_token();

					memoria->esperar_token();
				}

				send_token(memory);
			} catch (CommError & error) {
				Log::crit(error.what());
				break;
			} catch (OSError & error) {
				Log::crit(error.what());
				break;
			}
		}
	}
	leave();
}

void LocalBrokerComm::run() {

	if (tipo_de_recurso == SEMAFORO) {
		loop_semaforo();
	} else if (tipo_de_recurso == MUTEX) {
		loop_mutex();
	} else if (tipo_de_recurso == SHAREDMEMORY) {
		loop_memoria();
	}
}

/*
 void test_local_broker_comm2(int argc, char * argv []) {
 char *mem = new char [10 * 1024];

 if (argc < 3) {
 std::cerr << "Faltan argumentos (direcorio_de_trabajo,nombre_app,grupo)" << std::endl;
 return;
 }
 std::vector<std::string> brokers;
 std::vector<std::string> servicios;
 brokers.push_back("localbroker1.sitio1.aeropuerto1");
 servicios.push_back("1234");

 LocalBrokerComm broker(argv [1], argv [2], argv [3], brokers, servicios);

 for (int i = 0 ; i < 3 ; i++) {

 broker.wait_mutex(mem);

 broker.free_mutex(mem);
 }
 delete mem;
 }*/

void print_args(int argc, char * argv []) {
	int i;
	std::cout << "argc=" << argc << std::endl;
	std::cout << argv [0];
	for (i = 1; i < argc ; i++) {
		std::cout << " " << argv [i];
	}
	std::cout << std::endl;
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

int main(int argc, char * argv [])
try
{
	char grupo [MAX_NOMBRE_RECURSO];
	size_t tamanio;
	char id;
	std::vector<std::string> brokers;
	std::vector<std::string> servicios;
	tipo_ipc_t tipo;
	int num_sem;
	int cant_sem;

//print_args(argc,argv);
#if DEBUG_LOCAL_BROKER_COMM == 1
	chdir("processes");
#endif
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

#if DEBUG_LOCAL_BROKER_COMM ==1
	std::string args("argc= ");
	std::stringstream ss;
	ss << argc;
	args.append(ss.str());
	for (int i = 0 ; i < argc ; i++) {
		args.append(" ").append(argv [i]);
	}
	Log::info("%s", args.c_str());
#endif
	obtener_lista_de_brokers(brokers, servicios, argv [3]);

	strncpy(grupo, argv [4], MAX_NOMBRE_RECURSO);
	id = atoi(argv [5]);

#ifdef __x86_64__
	sscanf(argv [6], "%lu", &tamanio);
#else
	sscanf(argv [6], "%u", &tamanio);
#endif

	if (tamanio == 0) {
		if (strncmp(argv [7], "mutex", strlen("mutex")) == 0) {
			tipo = MUTEX;
		} else if (strncmp(argv [7], "sem", strlen("sem")) == 0) {
			tipo = SEMAFORO;
			num_sem = atoi(argv [8]);
			cant_sem = atoi(argv [9]);
		}
	} else {
		tipo = SHAREDMEMORY;
	}

	LocalBrokerComm comunicacion(argv [1], argv [2], argv [4], brokers, servicios, tipo, num_sem, cant_sem);

	comunicacion.run();

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

