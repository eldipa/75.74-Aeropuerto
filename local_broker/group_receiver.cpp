/*
 * GroupReceiver.cpp
 *
 *  Created on: 17/02/2013
 *      Author: gonzalo
 */

#include "group_receiver.h"


#include <cstring>
#include <cstdio>
#include <iostream>
#include <cstdlib>
#include "local_broker_constants.h"
#include "messagequeue.h"
#include "grupo.h"
#include "oserror.h"
#include "genericerror.h"

GroupReceiver::GroupReceiver(const std::string & directorio_de_trabajo, const std::string & nombre_grupo, char id,
	const std::string & nombre_broker_remoto)
	: cola_token_manager(std::string(directorio_de_trabajo).append(PATH_COLA_TOKEN_MANAGER).c_str(), char(0)),
		grupo_remoto(std::string(directorio_de_trabajo).append(PATH_COLAS_BROKERS).c_str(), id),
		grupo(directorio_de_trabajo, nombre_grupo), broker_remoto(nombre_broker_remoto)
{

}

GroupReceiver::~GroupReceiver() {

}

void GroupReceiver::send_token() {

}

void GroupReceiver::loop_token() {
	bool leave = false;
	/*int a;
	 char data [DATA_SIZE];*/
	do {
		try {
			// espero el token
			grupo.lock_token();
			/*std::cout << (char*)grupo->memory_pointer() << std::endl;
			 sscanf((char*)grupo->memory_pointer(), "%[^:]:%d", data, &a);
			 a++;
			 snprintf((char*)grupo->memory_pointer(), DATA_SIZE, "%s:%d", "client_handler", a);
			 std::cout << (char*)grupo->memory_pointer() << std::endl;*/

			// envio el token al cliente
			send_token();

		} catch (OSError & error) {
			leave = true;
		}
		if (leave) {
			// tengo que salir del grupo antes de liberar el token
			grupo.leave(broker_remoto.c_str());
		}
	} while (!leave);
}

void GroupReceiver::run() {
	loop_token();
}

int main(int argc, char * argv []) {
	char id;
	if (argc != 5) {
		std::cerr << "Falta el directorio de trabajo, el id, el nombre del recurso, nombre_broker_remoto" << std::endl;
		return -1;
	}
	std::string lock_file(argv [1]);
	lock_file.append(PATH_COLAS_BROKERS);
	id = atoi(argv [2]);

	GroupReceiver handler(argv [1], argv [3], id, argv [4]);

	handler.run();

	/*char data [100];
	//GroupInterface grupo_remoto(lock_file.c_str(), id);
	Grupo grupo(argv [1], argv [3]);
	grupo.join(argv [4]);

	for (int i = 0 ; i < 10 ; i++) {

		//grupo_remoto.pull(data, sizeof(data));

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
