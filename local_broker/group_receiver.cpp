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

GroupReceiver::GroupReceiver(const std::string & directorio_de_trabajo, const std::string & nombre_grupo, char id/*,
 const std::string & nombre_broker_local*/)
	: cola_token_manager(std::string(directorio_de_trabajo).append(PATH_COLA_TOKEN_MANAGER).c_str(), char(0)),
		grupo_remoto(std::string(directorio_de_trabajo).append(PATH_COLAS_BROKERS).c_str(), id),
		grupo(directorio_de_trabajo, nombre_grupo)/*,broker_local(nombre_broker_local)*/
{
	size_t tamanio;
	tamanio = grupo.get_mem_size();
	data_token = new char [tamanio];
	cantidad_recibida_token = 0;
	cantidad_recibida_grupo = 0;
	cantidad_de_bloques_por_token = (grupo.get_mem_size() / DATA_SIZE);
	if (grupo.get_mem_size() % DATA_SIZE != 0 || grupo.get_mem_size() == 0 || grupo.get_mem_size() < DATA_SIZE) {
		cantidad_de_bloques_por_token++;
	}
}

GroupReceiver::~GroupReceiver() {
	delete [] data_token;
}

size_t GroupReceiver::recv_token() {
	bool mensaje_completo = false;

	do {

		grupo_remoto.pull((char*)&mensaje, sizeof(mensajes::mensajes_local_broker_group_t));

		if (mensaje.tipo == mensajes::TOKEN_DELIVER) {
			memcpy((data_token + mensaje.numero_de_mensaje * DATA_SIZE), mensaje.data,
				std::min(grupo.get_mem_size() - mensaje.numero_de_mensaje * DATA_SIZE, size_t(DATA_SIZE)));
			cantidad_recibida_token += DATA_SIZE;
			if (cantidad_recibida_token >= mensaje.cantidad_bytes_total) {
				mensaje_completo = true;
				tipo_mensaje_recibido = mensajes::TOKEN_DELIVER;
				cantidad_recibida_token = 0;
			}
		} else if (mensaje.tipo == mensajes::GROUP_DISCOVER) {
			memcpy(data_group + mensaje.numero_de_mensaje * DATA_SIZE, mensaje.data, DATA_SIZE);
			cantidad_recibida_grupo += DATA_SIZE;
			if (cantidad_recibida_grupo >= mensaje.cantidad_bytes_total) {
				mensaje_completo = true;
				tipo_mensaje_recibido = mensajes::GROUP_DISCOVER;
				cantidad_recibida_grupo = 0;
			}
		}
	} while (!mensaje_completo);

	return 0;
}

void GroupReceiver::procesar_mensaje() {
	if (tipo_mensaje_recibido == mensajes::TOKEN_DELIVER) {
		memcpy(grupo.memory_pointer(), data_token, grupo.get_mem_size());
		grupo.release_token(&this->cola_token_manager);
	} else if (mensaje.tipo == mensajes::GROUP_DISCOVER) {
		memcpy(mensaje.data, data_group, DATA_SIZE);
		grupo_remoto.push((char *)&mensaje, sizeof(mensajes::mensajes_local_broker_group_t));
	}
}

void GroupReceiver::loop_token() {
	bool leave = false;
	grupo.release_token(&this->cola_token_manager);
	do {
		try {
			recv_token();

			std::cout << "Recibido:" << mensaje.data << std::endl;

			procesar_mensaje();

		} catch (OSError & error) {
			leave = true;
		}
	} while (!leave);
}

void GroupReceiver::run() {
	std::cout << data_token << std::endl;
	loop_token();
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

int main(int argc, char * argv []) {
	char id;

	print_args(argc, argv);

	if (argc < 5) {
		std::cerr << "Falta el directorio de trabajo, el id, el nombre del recurso, el nombre_broker_local"
			<< std::endl;
		return -1;
	}
	id = atoi(argv [2]);

	GroupReceiver handler(argv [1], argv [3], id/*, argv [4]*/);

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
	return 0;
}
