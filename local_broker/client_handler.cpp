/*
 * client_handler.cpp
 *
 *  Created on: 05/02/2013
 *      Author: gonzalo
 */

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

#include "client_handler.h"

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
}

ClientHandler::~ClientHandler() {
	if (mem_local) {
		delete [] mem_local;
	}
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
	//Log::info("Joining: %s to %s\n", this->nombre_cliente.c_str(), nombre_grupo.c_str());
	//std::cout << "Joining: " << this->nombre_cliente << " to " << nombre_grupo << std::endl;
	//cola_token_manager.push(&mensaje, sizeof(traspaso_token_t) - sizeof(long));
	cola_token_manager.push(&mensaje, len);
}

void ClientHandler::join_group(const std::string & nombre_grupo) {
	size_t tamanio;
	this->nombre_grupo = nombre_grupo;

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
	// Saco al cliente del grupo
	mensajes::mensajes_local_broker_token_t mensaje;
	grupo->leave(nombre_cliente.c_str());
	mensaje.respuesta = mensajes::LEAVE_OK;
	try {
#if DEBUG_CLIENT_HANDLER==1
		std::cout << nombre_cliente << " " << nombre_grupo << " Replying LEAVE" << std::endl;
#endif
		socket.sendsome(&mensaje, sizeof(mensajes::mensajes_local_broker_token_t));
	} catch (OSError & err) {

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

void ClientHandler::send_token() {
	mensajes::mensajes_local_broker_token_t mensaje;
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
	mensajes::mensajes_local_broker_token_t mensaje;
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
			if (mensaje.peticion == mensajes::LEAVED) {
				std::cout << nombre_cliente << "(" << nombre_grupo << ") LEAVED Received" << std::endl;
			}
			if (mensaje.peticion == mensajes::TOKEN_LEAVE) {
				std::cout << nombre_cliente << "(" << nombre_grupo << ") TOKEN_LEAVE Received" << std::endl;
			}
			if (this->nombre_grupo == "cinta_checkin" && i == 0) {
				std::cout << nombre_cliente << " Recibido: ";
				print_ints((int *)mensaje.datos, 9);
			}
#endif
			if (mensaje.peticion == mensajes::LEAVED) {
				return 0;
			}
			if (mensaje.peticion == mensajes::TOKEN_LEAVE) {
				leave = true;
			}

			if (leidos == 0) {
				Log::crit("El Cliente %s cerro la conexion\n", nombre_cliente.c_str());
#if DEBUG_CLIENT_HANDLER==1
				std::cout << "Broken Pipe " << this->nombre_cliente << "(" << nombre_grupo << ")" << std::endl;
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

void ClientHandler::loop_token_fork() {
	pid_t pid_hijo;
	ignore_signals();

	pid_hijo = fork();

	if (pid_hijo) { // padre envia token a cliente4
		ignore_childs();
		do {
			try {
				//Log::debug("Padre: Esperando token %s de %s\n", nombre_grupo.c_str(), nombre_cliente.c_str());
				if (recv_token() == 0) {
					leave = true;
				} else {
					grupo->release_token(&cola_token_manager);
				}
			} catch (OSError & error) {
				Log::crit(error.what());
				leave = true;
			} catch (CommError & error) {
				Log::crit(error.what());
				leave = true;
			}
			if (leave) {
				int status = -1;
				//leave_group();
				try {
					socket.disassociate();
				} catch (OSError & error) {
					Log::crit(error.what());
				}
				while (waitpid(pid_hijo, &status, 0) != -1) {
				}
			}
		} while (!leave);
	} else {
		bool tengo_token = false;
		do {
			try {
				grupo->lock_token();
				tengo_token = true;
				//	Log::debug("Hijo: Enviando token %s a %s\n", nombre_grupo.c_str(), nombre_cliente.c_str());
				send_token();
				tengo_token = false;
			} catch (OSError & error) {
				Log::crit(error.what());
				leave = true;
			} catch (CommError & error) {
				Log::crit(error.what());
				leave = true;
			}
		} while (!leave);
		leave_group();
		if (tengo_token) {
			grupo->release_token(&cola_token_manager);
		}
	}
}

void ClientHandler::loop_token() {
	bool tengo_el_token = false;
	/*int a;
	 char data [DATA_SIZE];*/

	if (tipo_join == mensajes::JOIN_FORK) {
		loop_token_fork();
	} else {
		do {
			try {
				// espero el token
				grupo->lock_token();
				tengo_el_token = true;
				/*if (nombre_grupo == "cinta_escaner_control") {
				 std::cout << "sent: ";
				 print_ints((int*)grupo->memory_pointer(), int(grupo->get_mem_size()/ 4));
				 }*/
				/*std::cout << (char*)grupo->memory_pointer() << std::endl;
				 sscanf((char*)grupo->memory_pointer(), "%[^:]:%d", data, &a);
				 a++;
				 snprintf((char*)grupo->memory_pointer(), DATA_SIZE, "%s:%d", "client_handler", a);
				 std::cout << (char*)grupo->memory_pointer() << std::endl;*/

				// envio el token al cliente
				send_token();

				// espero que devuelva el token
				if (recv_token() == 0) {
					leave = true;
				}
				/*if (nombre_grupo == "cinta_escaner_control") {
				 std::cout << "recv: ";
				 print_ints((int*)grupo->memory_pointer(), int(grupo->get_mem_size() / 4));
				 }*/
			} catch (OSError & error) {
				Log::crit(error.what());
				leave = true;
			}
			if (leave) {
				// tengo que salir del grupo antes de liberar el token
				leave_group();
			}
			if (tengo_el_token) {
				// libero el token
				grupo->release_token(&cola_token_manager);
			}
			tengo_el_token = false;
		} while (!leave);
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
