/*
 * group_comm_inbound.cpp
 *
 *  Created on: 15/02/2013
 *      Author: gonzalo
 */

#include "group_sender.h"
#include "messagequeue.h"
#include <cstring>
#include <cstdio>
#include <iostream>
#include <cstdlib>
#include "local_broker_constants.h"
#include "grupo.h"
#include "oserror.h"
#include "genericerror.h"

GroupSender::GroupSender(const std::string & directorio_de_trabajo,
		const std::string & nombre_grupo, char id,
		const std::string & nombre_broker_local) :
		grupo_remoto(NULL), grupo(directorio_de_trabajo, nombre_grupo), broker_local(
				nombre_broker_local) {

	for (int i = 0; i < 3; i++) {
		try {
			grupo_remoto = new GroupInterface(
					std::string(directorio_de_trabajo).append(
							PATH_COLAS_BROKERS).c_str(), id);
			break;
		} catch (OSError & error) {
			sleep(1);
			if (i == 3) {
				throw error;
			}
		}
	}

	tamanio_memoria = grupo.get_mem_size();

	cantidad_de_bloques_por_token = (tamanio_memoria / DATA_SIZE);

	if (tamanio_memoria % DATA_SIZE
			!= 0|| tamanio_memoria == 0 || tamanio_memoria < DATA_SIZE) {cantidad_de_bloques_por_token
		++;
	}

	mensaje.tipo = mensajes::TOKEN_DELIVER;
	mensaje.cantidad_bytes_total = tamanio_memoria;

	grupo.join(nombre_broker_local.c_str());
}

GroupSender::~GroupSender() {
	if (grupo_remoto) {
		delete grupo_remoto;
	}
}

void GroupSender::send_token(int tipo) {
	int i;
	if(tipo == 1){
		mensaje.tipo = mensajes::TOKEN_DELIVER;
	}else if(tipo==2){
		mensaje.tipo = mensajes::MEMORY_UPDATE;
	}

	for (i = 0; i < cantidad_de_bloques_por_token; i++) {
		mensaje.numero_de_mensaje = i;
		memcpy(mensaje.data, (char*) grupo.memory_pointer() + i * DATA_SIZE,
				DATA_SIZE);
		sprintf(mensaje.data, "%d", i);
		grupo_remoto->push((char*) &mensaje,
				sizeof(mensajes::mensajes_local_broker_group_t));
	}
	grupo.el_token_se_envio();
}

void GroupSender::loop_token() {
	bool leave = false;
	/*int a;
	 char data [DATA_SIZE];*/

	// DEBUG MENSAJERIA 1
	/*mensaje.numero_de_mensaje = 0;
	 mensaje.cantidad_bytes_total = DATA_SIZE;
	 strcpy(mensaje.data,"Prueba LOOP 1 Nodo");
	 mensaje.tipo = mensajes::GROUP_DISCOVER;
	 grupo_remoto->push((char*) &mensaje,
	 sizeof(mensajes::mensajes_local_broker_group_t));

	 mensaje.tipo = mensajes::TOKEN_DELIVER;*/
	int tipo;
	do {
		try {
			// espero el token
			tipo = grupo.lock_token();
			/*std::cout << (char*)grupo->memory_pointer() << std::endl;
			 sscanf((char*)grupo->memory_pointer(), "%[^:]:%d", data, &a);
			 a++;
			 snprintf((char*)grupo->memory_pointer(), DATA_SIZE, "%s:%d", "client_handler", a);
			 std::cout << (char*)grupo->memory_pointer() << std::endl;*/

			// envio el token al client
			send_token(tipo);
			std::cout << "Token Enviado" << std::endl;

		} catch (OSError & error) {
			leave = true;
		}
		if (leave) {
			// tengo que salir del grupo antes de liberar el token
			grupo.leave(broker_local.c_str());
		}
	} while (!leave);
}

void GroupSender::run() {
	loop_token();
}

void print_args(int argc, char * argv[]) {
	int i;
	std::cout << "argc=" << argc << std::endl;
	std::cout << argv[0];
	for (i = 1; i < argc; i++) {
		std::cout << " " << argv[i];
	}
	std::cout << std::endl;
}

int main(int argc, char * argv[]) {
	char id;

	print_args(argc, argv);

	if (argc < 5) {
		std::cerr
				<< "Falta el directorio de trabajo, el id, el nombre del recurso, nombre_broker_local"
				<< std::endl;
		return -1;
	}
	id = atoi(argv[2]);

	GroupSender handler(argv[1], argv[3], id, argv[4]);

	handler.run();

	/*char data [100];
	 //GroupInterface grupo_remoto(lock_file.c_str(), id);
	 Grupo grupo(argv [1], argv [3]);
	 grupo.join(argv [4]);

	 for (int i = 0 ; i < 10 ; i++) {

	 //grupo_remoto->pull(data, sizeof(data));

	 std::cout << "recibido: " << data << std::endl;

	 }*/

	/*
	 MessageQueue outbound("/tmp/test", char(128));
	 MessageQueue inbound("/tmp/test", char(0));
	 mensaje_t mensaje;

	 mensaje.mtype = 1;

	 for (int i = 0; i < 10; i++) {
	 sprintf(mensaje.msg, "hola%d", i);

	 std::cout << "enviado:" << mensaje.msg << std::endl;

	 outbound.push(&mensaje, size_t(sizeof(mensaje_t) - sizeof(long)));
	 }*/
	return -1;
}

