/*
 * client_handler.cpp
 *
 *  Created on: 05/02/2013
 *      Author: gonzalo
 */

#include "client_handler.h"
#include <iostream>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include "log.h"
#include "local_broker.h"
#include "mensajes_de_red.h"
#include "messagequeue.h"
#include "oserror.h"
#include <sys/wait.h>
#include <unistd.h>
#include "daemon.h"
#include "commerror.h"
#include "interrupted.h"

void print_ints(int *p, int cant) {
	std::cout << p [0];
	for (int i = 1 ; i < cant ; i++) {
		std::cout << " " << p [i];
	}
	std::cout << std::endl;
}

ClientHandler::ClientHandler(const std::string & directorio_de_trabajo, /*char id,*/
int fd)
	: socket(fd), directorio_de_trabajo(directorio_de_trabajo),
		cola_token_manager(std::string(directorio_de_trabajo).append(PATH_COLA_TOKEN_MANAGER).c_str(), char(0))
{
	mem_local = NULL;
	tipo_join = mensajes::JOIN;
	leave = false;
	salir = 0;

	SignalHandler::getInstance()->registrarHandler(SIGUSR1, this);
	SignalHandler::getInstance()->registrarHandler(SIGUSR2, this);
	SignalHandler::getInstance()->registrarHandler(SIGTERM, this);
}

ClientHandler::~ClientHandler() {
	if (mem_local) {
		delete [] mem_local;
	}
	SignalHandler::destruir();
}

void ClientHandler::avisar_creacion_grupo(const std::string & nombre_grupo) {
	size_t len;
	traspaso_token_t mensaje;
	// envio mensaje para que el token manager levante el grupo
	mensaje.mtype = 1;
	mensaje.tipo = 3; // crear grupo
	strncpy(mensaje.recurso, nombre_grupo.c_str(), MAX_NOMBRE_RECURSO);
	len = strlen(mensaje.recurso) + 1;
	len += sizeof(long);
	Log::info("Joining: %s to %s\n", this->nombre_cliente.c_str(), nombre_grupo.c_str());
	//std::cout << "Joining: " << this->nombre_cliente << " to " << nombre_grupo << std::endl;
	//cola_token_manager.push(&mensaje, sizeof(traspaso_token_t) - sizeof(long));
	cola_token_manager.push(&mensaje, len);
}

void ClientHandler::join_group(const std::string & nombre_grupo) {
	size_t tamanio;
	this->nombre_grupo = nombre_grupo;
	strncpy(mensaje.recurso, nombre_grupo.c_str(), MAX_NOMBRE_RECURSO);

	avisar_creacion_grupo(nombre_grupo);
	// Obtengo la memoria compartida del grupo
	grupo = new Grupo(directorio_de_trabajo, nombre_grupo);
	// Agrego el cliente y el numero de la cola
	grupo->join(nombre_cliente.c_str());
	std::string host;
	std::string servicio;

	socket.from_who(host, servicio);
#if DEBUG_CLIENT_HANDLER==1
	std::cout << "Cliente " << nombre_cliente << "(" << host << ") conectado a " << nombre_grupo << std::endl;
#endif
	// calculo la cantidad de bloques a enviar por token
	tamanio = grupo->get_mem_size();

	cantidad_de_bloques_a_enviar = (tamanio / DATA_SIZE);

	if (tamanio % DATA_SIZE != 0 || tamanio == 0 || tamanio < DATA_SIZE) {
		cantidad_de_bloques_a_enviar++;
	}
#if DEBUG_CLIENT_HANDLER==1
	std::cout << "Tamaño Memoria: " << nombre_grupo << " = " << tamanio << " cantidad de bloques = "
	<< cantidad_de_bloques_a_enviar << std::endl;
#endif
	tamanio_memoria = tamanio;

	if (tamanio != 0) {
		mem_local = new char [tamanio];
	}
}

void ClientHandler::leave_group() {
	Log::info("Leaving group %s(%s)", nombre_cliente.c_str(), nombre_grupo.c_str());
	grupo->leave(nombre_cliente.c_str());

}

void ClientHandler::handleSignal(int signum) {
	if (signum == SIGUSR1) {
		salir = 1;
#if DEBUG_CLIENT_HANDLER == 1
		std::cout << nombre_cliente << "(" << nombre_grupo << "-" << getpid() << "): señal recibida salir = 1"
		<< std::endl;
#endif
	} else if (signum == SIGUSR2) {
		salir = 2;
#if DEBUG_CLIENT_HANDLER == 1
		std::cout << nombre_cliente << "(" << nombre_grupo << "-" << getpid() << "): señal recibida salir = 2"
		<< std::endl;
#endif
	} else if (signum == SIGTERM) {
		salir = 3;
	}
}

void ClientHandler::procesar_peticion(mensajes::mensajes_local_broker_t & mensaje) {

	switch (mensaje.peticion) {
		case mensajes::REGISTER:
			//Log::debug("Register %s", mensaje.datos);
			//std::cout << "Register: " << mensaje.datos << std::endl;
			nombre_cliente.assign(mensaje.datos);
			break;
		case mensajes::JOIN:
		case mensajes::JOIN_FORK:

			//std::cout << "Join: " << mensaje.datos << std::endl;
			// buscar el grupo en el que está el recurso, si no existe crearlo
			join_group(mensaje.datos);
			tipo_join = mensaje.peticion;
#ifdef __x86_64__
			snprintf(mensaje.datos, DATA_SIZE, "%lu:%lu", this->cantidad_de_bloques_a_enviar, this->tamanio_memoria);
#else
			snprintf(mensaje.datos, DATA_SIZE, "%u:%u",
				this->cantidad_de_bloques_a_enviar, this->tamanio_memoria);
#endif
			mensaje.respuesta = mensajes::OK;
			break;
		case mensajes::LEAVE:
			//std::cout << "Leave: " << mensaje.datos << std::endl;
			// sacar al cliente del grupo
			mensaje.respuesta = mensajes::OK;
			break;
		default:
			break;
	}
	socket.sendsome(&mensaje, sizeof(mensajes::mensajes_local_broker_t));
}

void ClientHandler::send_token_sem() {

	mensaje.respuesta = mensajes::R_TOKEN;
	socket.sendsome(&mensaje, sizeof(mensajes::mensajes_local_broker_token_t));

}

void ClientHandler::recv_token_sem() {
	size_t leidos;
	try {
		leidos = socket.receivesome(((char *)&mensaje), sizeof(mensajes::mensajes_local_broker_token_t));
		tengo_token = 1;

		if (mensaje.peticion == mensajes::LEAVE) {
			salir = 1;
			tengo_token = 0;
		}

		if (leidos == 0) {
			tengo_token = 0;
			throw CommError("El cliente cerro la conexion\n");
		}
	} catch (InterruptedSyscall & interruption) {
		Log::alert(interruption.what());
		tengo_token = 0;
		if (salir != 3) {
			salir = 2;
		}
	}
}

void ClientHandler::send_token() {
	size_t i;
	for (i = 0; i < this->cantidad_de_bloques_a_enviar && !leave ; i++) {
		strncpy(mensaje.recurso, grupo->get_nombre_recurso().c_str(), MAX_NOMBRE_RECURSO);
		if (tamanio_memoria != 0) {
			memcpy(mensaje.datos, ((char *)grupo->memory_pointer() + i * DATA_SIZE),
				std::min(this->tamanio_memoria - i * DATA_SIZE, size_t(DATA_SIZE)));
		}
		socket.sendsome(&mensaje, sizeof(mensajes::mensajes_local_broker_token_t));
	}
}

size_t ClientHandler::recv_token() {
	size_t cant = 0;
	size_t leidos;
	size_t i;
	if (tamanio_memoria != 0) {
		bzero(mem_local, grupo->get_mem_size());
	}
	for (i = 0; i < this->cantidad_de_bloques_a_enviar && !leave ; i++) {
		cant = 0;
		do {
			leidos = socket.receivesome(((char *)&mensaje) + cant,
				sizeof(mensajes::mensajes_local_broker_token_t) - cant);
#if DEBUG_CLIENT_HANDLER==1
			if (mensaje.peticion == mensajes::LEAVE) {
				std::cout << nombre_cliente << "(" << nombre_grupo << ") Leave Received" << std::endl;
			}
			/*if (this->nombre_grupo == "cinta_checkin" && i == 0) {
				std::cout << nombre_cliente << " Recibido: ";
				print_ints((int *)mensaje.datos, 9);
			}*/
#endif
			if (mensaje.peticion == mensajes::LEAVE) {
				Log::info("%s(%s) Leave received", nombre_cliente.c_str(), nombre_grupo.c_str());
				return 0;
			}

			if (leidos == 0) {
				Log::alert("El Cliente %s(%s) cerro la conexion\n", nombre_cliente.c_str(), nombre_grupo.c_str());
#if DEBUG_CLIENT_HANDLER==1
				std::cout << "Saliendo " << this->nombre_cliente << "(" << nombre_grupo << ")" << std::endl;
#endif
				return 0;
			}
			cant += leidos;
		} while (cant < sizeof(mensajes::mensajes_local_broker_token_t) && !leave);
		if (tamanio_memoria != 0) {
			memcpy(mem_local + i * DATA_SIZE, mensaje.datos,
				std::min(tamanio_memoria - i * DATA_SIZE, size_t(DATA_SIZE)));
		}
	}
// actualizo la memoria con lo recibido
	if (tamanio_memoria != 0) {
		memcpy(grupo->memory_pointer(), mem_local, grupo->get_mem_size());
	}
	return this->cantidad_de_bloques_a_enviar * DATA_SIZE;
}

void ClientHandler::loop_semaforo_padre() {
	while (salir == 0) {
		try {
			//Log::debug("Padre: Esperando token %s de %s\n", nombre_grupo.c_str(), nombre_cliente.c_str());

			recv_token_sem();

		} catch (OSError & error) {
			Log::crit(error.what());
			break;
		} catch (CommError & error) { // el cliente cerro la conexion
			Log::alert(error.what());
			salir = 2;
			break;
		}

		if (tengo_token == 1) {
			grupo->release_token(&cola_token_manager);
			tengo_token = 0;
		}
	}
}

void ClientHandler::chequear_status_salida_hijo() {
	if (salir == 1) {
		if (grupo->tengo_el_token_cliente()) {
#if DEBUG_CLIENT_HANDLER==1
			std::cout << nombre_cliente << "(" << nombre_grupo << ") Chequeando si el cliente tiene el token"
			<< std::endl;
#endif
			mensaje.respuesta = mensajes::R_TOKEN_CHECK;
			try {
				socket.sendsome(&mensaje, sizeof(mensajes::mensajes_local_broker_token_t));
				socket.receivesome(((char *)&mensaje), sizeof(mensajes::mensajes_local_broker_token_t));
				if (mensaje.peticion == mensajes::TOKEN_LEAVE) {
					tengo_token = 1;
				} else {
					tengo_token = 0;
				}
				mensaje.respuesta = mensajes::LEAVE_OK;
#if DEBUG_CLIENT_HANDLER==1
				std::cout << nombre_cliente << "(" << nombre_grupo << ") Tiene el token: "
				<< ((tengo_token == 0) ? "True" : "False") << std::endl;
#endif
				socket.sendsome(&mensaje, sizeof(mensajes::mensajes_local_broker_token_t));
			} catch (OSError & err) {
			}
			if (tengo_token) {
				grupo->release_token(&cola_token_manager);
			} else if (chequear_asignacion == 1) {
#if DEBUG_CLIENT_HANDLER==1
				std::cout << nombre_cliente << "(" << nombre_grupo << ") extrayendo mensaje de la cola" << std::endl;
#endif
				grupo->lock_token();
#if DEBUG_CLIENT_HANDLER==1
				std::cout << nombre_cliente << "(" << nombre_grupo << ") liberando token" << std::endl;
#endif
				grupo->release_token(&cola_token_manager);
			}
		}
	} else if (salir == 2) {
		Log::info("%s(%s) Saliendo", nombre_cliente.c_str(), nombre_grupo.c_str());
#if DEBUG_CLIENT_HANDLER==1
		std::cout << nombre_cliente << "(" << nombre_grupo << ") Broken Pipe" << std::endl;
#endif
		if (grupo->tengo_el_token_cliente()) {
			grupo->release_token(&cola_token_manager);
		}
	}
}

void ClientHandler::loop_semaforo_hijo() {
	chequear_asignacion = 0;
	while (salir == 0) {

		try {
			grupo->lock_token();
			tengo_token = 1;
		} catch (InterruptedSyscall & interruption) {
			Log::alert(interruption.what());
			tengo_token = 0;
			chequear_asignacion = 1;
		}

		try {
			if (tengo_token == 1) {
				send_token_sem();
				tengo_token = 0;
			}
		} catch (OSError & error) {
			Log::crit(error.what());
			if (salir != 3) {
				salir = 2;
			}
			tengo_token = 1;
		} catch (InterruptedSyscall & interruption) {
			Log::alert(interruption.what());
			tengo_token = 1;
			chequear_asignacion = 0;
		}
	}

}

void ClientHandler::loop_semaforo() {
	pid_t pid_hijo;
	ignore_signals();

	pid_hijo = fork();

	if (pid_hijo) { // padre envia token a cliente
		int status = -1;
		ignore_childs();
		loop_semaforo_padre();
		leave_group();
		if (salir == 2) {
			kill(pid_hijo, SIGUSR2);
		} else if (salir == 1) {
			kill(pid_hijo, SIGUSR1);
		} else if (salir == 3) {
			kill(pid_hijo, SIGTERM);
		}
#if DEBUG_CLIENT_HANDLER==1
		std::cout << nombre_cliente << "(" << nombre_grupo << ") esperando hijo" << std::endl;
#endif
		Log::info("%s(%s) esperando hijo", nombre_cliente.c_str(), nombre_grupo.c_str());
		waitpid(pid_hijo, &status, 0);
#if DEBUG_CLIENT_HANDLER==1
		std::cout << nombre_cliente << "(" << nombre_grupo << ") hijo termino" << std::endl;
#endif
		Log::info("%s(%s) fin hijo", nombre_cliente.c_str(), nombre_grupo.c_str());
	} else {
		loop_semaforo_hijo();
		chequear_status_salida_hijo();
	}
}

void ClientHandler::loop_memoria() {
	bool tengo_el_token = false;
	do {
		try {
			tengo_el_token = false;
			grupo->lock_token();
			tengo_el_token = true;

		} catch (InterruptedSyscall & interruption) {
			Log::alert(interruption.what());
			tengo_el_token = false;
			leave = true;
		}
		try {
			send_token();

			if (recv_token() == 0) {
				leave = true;
			}

		} catch (OSError & error) {
#if DEBUG_CLIENT_HANDLER==1
			std::cout << nombre_cliente << "(" << nombre_grupo << ") Saliendo" << std::endl;
#endif
			Log::alert(error.what());
			leave = true;
		}

		try {
			if (leave) {
				leave_group();
			}
			if (tengo_el_token) {
				grupo->release_token(&cola_token_manager);
			}
			tengo_el_token = false;
		} catch (OSError & error) {
		}
	} while (!leave);
	try {
		mensaje.respuesta = mensajes::LEAVE_OK;
		socket.sendsome(&mensaje, sizeof(mensajes::mensajes_local_broker_token_t));
	} catch (OSError & error) {
	}
}

void ClientHandler::loop_token() {

	if (tipo_join == mensajes::JOIN_FORK) {
		loop_semaforo();
	} else {
		loop_memoria();
	}
}

void ClientHandler::run() {
	mensajes::mensajes_local_broker_t mensaje;

	do {
		try {

			if (socket.receivesome(&mensaje, sizeof(mensajes::mensajes_local_broker_t)) == 0) {
				mensaje.peticion = mensajes::LEAVE;
			} else {
				this->procesar_peticion(mensaje);
			}
		} catch (OSError & error) {
			mensaje.peticion = mensajes::LEAVE;
			//std::cerr << error.what() << std::endl;
			Log::crit(error.what());
		}

	} while (mensaje.peticion != mensajes::LEAVE && mensaje.peticion != mensajes::JOIN
		&& mensaje.peticion != mensajes::JOIN_FORK);

	if (mensaje.peticion != mensajes::LEAVE) {
		loop_token();
	}
}

int main(int argc, char * argv [])
try
{
	if (argc != 4) {
		std::cerr << "Falta el socket" << std::endl;
		return -1;
	}
	int fd;
//char id;
//std::cout << "client_handler " << argv [1] << " " << argv [2] << " " << argv [3] << std::endl;

	fd = atoi(argv [2]);
//id = atoi(argv [3]);

///// SOCKET ESCUCHA SOLO PARA DEBUG
	/*int new_socket;
	 std::string puerto("1234");
	 Socket * server_socket = new Socket(true);
	 server_socket->source(puerto);
	 new_socket = server_socket->listen_fd(10);
	 fd = new_socket;
	 server_socket->disassociate();
	 delete server_socket;*/

	ClientHandler handler(argv [1], /*id,*/fd);

	handler.run();

	/*socket.receivesome(debug, 200);
	 std::cout << "Recibido: " << debug << std::endl;
	 snprintf(debug, 200, "%s", "RECIBIDO!");
	 socket.sendsome(debug, strlen(debug));*/

}
catch (const std::exception & e) {
//std::cerr << "Error Broker Local" << std::endl;
//std::cerr << e.what() << std::endl;
	Log::crit("%s", e.what());
}
catch (...) {
//std::cerr << "Error Broker Local" << std::endl;
	Log::crit("Critical error. Unknow exception at the end of the 'main' function.");
}
