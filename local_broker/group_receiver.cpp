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
#include "process.h"
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

static char id [4];
static char directorio_trabajo [FILENAME_MAX];
static char nombre_del_grupo [MAX_NOMBRE_RECURSO];
static char nombre_broker_local [MAX_NOMBRE_RECURSO];

static char * args_lider [] = {(char*)"lider", directorio_trabajo, id, nombre_del_grupo, nombre_broker_local};

GroupReceiver::GroupReceiver(const std::string & directorio_de_trabajo, const std::string & nombre_grupo, char id,
	const std::string & nombre_broker_local)
	: cola_token_manager(std::string(directorio_de_trabajo).append(PATH_COLA_TOKEN_MANAGER).c_str(), char(0)),
		grupo_remoto(std::string(directorio_de_trabajo).append(PATH_COLAS_BROKERS).c_str(), id),
		grupo(directorio_de_trabajo, nombre_grupo), directorio_de_trabajo(directorio_de_trabajo),
		broker_local(nombre_broker_local), nombre_recurso(nombre_grupo), id_grupo(id)
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
	leader = false;

	numero_anterior = -1;
	leader_q = NULL;
}

GroupReceiver::~GroupReceiver() {
	delete [] data_token;
	if (leader_q != NULL) {
		delete leader_q;
	}
}

void GroupReceiver::inicializar() {
	bool inicializado = false;
	mensaje.tipo = mensajes::INITALIZATION;
	mensaje.cantidad_bytes_total = strlen(this->broker_local.c_str());
	strcpy(mensaje.data, this->broker_local.c_str());
	mensaje.numero_de_mensaje = 0;

	grupo_remoto.push((char *)&mensaje, sizeof(mensajes::mensajes_local_broker_group_t));
	token_inicializado = false;
	while (!inicializado) {
		grupo_remoto.pull((char*)&mensaje, sizeof(mensajes::mensajes_local_broker_group_t));

		if (mensaje.tipo == mensajes::LEADER) { // soy el lider
			leader = true;
		} else if (mensaje.tipo == mensajes::INITALIZATION) {
			if (strcmp(mensaje.data, this->broker_local.c_str()) == 0) {
				if (mensaje.numero_de_mensaje == 0) {
					// no esta inicializada, inicializo y genero el token (tendria que ser el lider)
					if (leader) {
						char path [FILENAME_MAX];
						strcpy(path, directorio_de_trabajo.c_str());
						strcat(path, "/");
						strcat(path, nombre_recurso.c_str());
						strcat(path, POSTFIJO_INIT);
						grupo.inicializar_memoria(path);
						grupo.release_token(&cola_token_manager);
						token_inicializado = true;
					}
					inicializado = true;
				} else {
					inicializado = true;
					token_inicializado = true;
				}
			} else {
				grupo_remoto.push((char *)&mensaje, sizeof(mensajes::mensajes_local_broker_group_t));
			}
		} else {
			grupo_remoto.push((char *)&mensaje, sizeof(mensajes::mensajes_local_broker_group_t));
		}
	}
}

void GroupReceiver::launch_lider() {
	struct stat buf;
	char path [200];
	strcpy(path, directorio_de_trabajo.c_str());
	strcat(path, PATH_COLA_LEADER);
	int file;
	if (stat(path, &buf) != 0) {
		file = open(path, O_CREAT | 0664);
		if (file != -1) {
			close(file);
		} else {
			//THROW OSERROR
		}
	}
	strcpy(directorio_trabajo, directorio_de_trabajo.c_str());
	sprintf(id, "%d", id_grupo);
	strcpy(nombre_del_grupo, nombre_recurso.c_str());
	strcpy(nombre_broker_local, broker_local.c_str());
	this->leader_q = new MessageQueue(path, char(0), 0664, true);
	Process p("lider", args_lider);
}

size_t GroupReceiver::recv_token() {
	bool mensaje_completo = false;
	bool token_roto = false;
	do {
		grupo_remoto.pull((char*)&mensaje, sizeof(mensajes::mensajes_local_broker_group_t));

		if (mensaje.tipo == mensajes::TOKEN_DELIVER) {
			// Chequeo que no se haya perdido la primera parte de un token
			if (mensaje.numero_de_mensaje == 0) {
				numero_anterior = -1;
				token_roto = false;
			}
			if (mensaje.numero_de_mensaje != numero_anterior + 1) {
				mensaje.tipo = mensajes::TOKEN_LOST;
				int * entero;
				entero = (int *)&mensaje.data;
				*entero = this->numero_de_nodo;
				//strncpy(mensaje.data, this->broker_local.c_str(), MAX_NOMBRE_RECURSO);
				grupo_remoto.push((char *)&mensaje, sizeof(mensajes::mensajes_local_broker_group_t));
				token_roto = true;
				cantidad_recibida_token += DATA_SIZE;
			} else {
				memcpy((data_token + mensaje.numero_de_mensaje * DATA_SIZE), mensaje.data,
					std::min(grupo.get_mem_size() - mensaje.numero_de_mensaje * DATA_SIZE, size_t(DATA_SIZE)));
				cantidad_recibida_token += DATA_SIZE;
				if (cantidad_recibida_token >= mensaje.cantidad_bytes_total) {
					mensaje_completo = true;
					tipo_mensaje_recibido = mensajes::TOKEN_DELIVER;
					cantidad_recibida_token = 0;
					numero_anterior = -1;
					this->token_inicializado = true;
				} else {
					numero_anterior++;
				}
			}
		} else if (mensaje.tipo == mensajes::GROUP_DISCOVER) {
			memcpy(data_group + mensaje.numero_de_mensaje * DATA_SIZE, mensaje.data, DATA_SIZE);
			cantidad_recibida_grupo += DATA_SIZE;
			if (cantidad_recibida_grupo >= mensaje.cantidad_bytes_total) {
				mensaje_completo = true;
				tipo_mensaje_recibido = mensajes::GROUP_DISCOVER;
				cantidad_recibida_grupo = 0;
			}
		} else {
			mensaje_completo = true;
		}
	} while (!mensaje_completo);

	return 0;
}

void GroupReceiver::procesar_mensaje() {
	if (mensaje.tipo == mensajes::TOKEN_DELIVER) {
		memcpy(grupo.memory_pointer(), data_token, grupo.get_mem_size());
		grupo.release_token(&this->cola_token_manager);
	} else if (mensaje.tipo == mensajes::GROUP_DISCOVER) {
		//memcpy(mensaje.data, data_group, DATA_SIZE);
		//grupo_remoto.push((char *)&mensaje, sizeof(mensajes::mensajes_local_broker_group_t));
		if (leader) {
			mensajes::mensajes_local_broker_lider_t msg;
			msg.mtype = 1;
			memcpy(&msg.data, &mensaje, sizeof(mensajes::mensajes_local_broker_group_t));
			leader_q->push((char *)&mensaje, sizeof(mensajes::mensajes_local_broker_group_t));
		} else {
			int * entero;
			entero = (int *)&mensaje.data;
			(*entero)++;
			this->numero_de_nodo = *entero;
			grupo_remoto.push((char *)&mensaje, sizeof(mensajes::mensajes_local_broker_group_t));
		}
	} else if (mensaje.tipo == mensajes::INITALIZATION) {
		mensaje.numero_de_mensaje++;
		grupo_remoto.push((char *)&mensaje, sizeof(mensajes::mensajes_local_broker_group_t));
	} else if (mensaje.tipo == mensajes::LEADER && !this->leader) {
		this->leader = true;
		if (!this->token_inicializado) {
			grupo.release_token(&cola_token_manager);
		}
		launch_lider();
	} else if (mensaje.tipo == mensajes::TOKEN_RECOVER) {
		if (leader) {
			mensajes::mensajes_local_broker_lider_t msg;
			msg.mtype = 1;
			memcpy(&msg.data, &mensaje, sizeof(mensajes::mensajes_local_broker_group_t));
			leader_q->push((char *)&mensaje, sizeof(mensajes::mensajes_local_broker_group_t));
		} else if (strcmp(mensaje.data, "OK") != 0) {
			int * entero;
			entero = (int *)&mensaje.data;
			if (*entero == this->numero_de_nodo) {
				grupo.reenviar_token_al_cliente();
				strcpy(mensaje.data, "OK");
			}
		}
	}
}

void GroupReceiver::loop_token() {
	bool leave = false;
//grupo.release_token(&this->cola_token_manager);
	do {
		try {
			recv_token();

			procesar_mensaje();

		} catch (OSError & error) {
			leave = true;
		}
	} while (!leave);
}

void GroupReceiver::run() {
	inicializar();
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

//print_args(argc, argv);

	if (argc < 5) {
		std::cerr << "Falta el directorio de trabajo, el id, el nombre del recurso, el nombre_broker_local"
			<< std::endl;
		return -1;
	}
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
	return 0;
}
