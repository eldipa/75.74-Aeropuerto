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
#include "mensajes_de_red.h"

static char id[4];
static char directorio_trabajo[FILENAME_MAX];
static char nombre_del_grupo[MAX_NOMBRE_RECURSO];
static char nombre_broker_local[MAX_NOMBRE_RECURSO];

//static char * args_lider [] = {(char*)"lider", directorio_trabajo, id, nombre_del_grupo, nombre_broker_local};

GroupReceiver::GroupReceiver(const std::string & directorio_de_trabajo,
		const std::string & nombre_grupo, char id,
		const std::string & nombre_broker_local, bool lanzar_token) :
		cola_token_manager(
				std::string(directorio_de_trabajo).append(
						PATH_COLA_TOKEN_MANAGER).c_str(), char(0)), grupo_remoto(
				NULL), grupo(directorio_de_trabajo, nombre_grupo), directorio_de_trabajo(
				directorio_de_trabajo), broker_local(nombre_broker_local), nombre_recurso(
				nombre_grupo), id_grupo(id), crear_token(lanzar_token) {
	size_t tamanio;

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
	tamanio = grupo.get_mem_size();
	data_token = new char[tamanio];
	data_replica = new char[tamanio];
	mutex = true;
	if (tamanio > 0) {
		mutex = false;
	}
	cantidad_recibida_token = 0;
	cantidad_recibida_grupo = 0;
	cantidad_de_bloques_por_token = (grupo.get_mem_size() / DATA_SIZE);
	if (grupo.get_mem_size() % DATA_SIZE
			!= 0|| grupo.get_mem_size() == 0 || grupo.get_mem_size() < DATA_SIZE) {cantidad_de_bloques_por_token++;
}
	leader = false;
	memoria_inicializada = false;

	numero_anterior = -1;
	leader_q = NULL;
}

GroupReceiver::~GroupReceiver() {
	delete[] data_token;
	delete[] data_replica;
	if (leader_q != NULL) {
		delete leader_q;
	}
	if (grupo_remoto) {
		delete grupo_remoto;
	}
}

void GroupReceiver::entregar_token_a_aplicacion() {
	std::cout << "Token Recibido" << std::endl;
	memcpy(grupo.memory_pointer(), data_token, grupo.get_mem_size());
	grupo.release_token(&this->cola_token_manager);
}

void GroupReceiver::inicializando() {
	bool estoy_inicializando = true;
	mensaje.tipo = mensajes::INITIALIZATION;
	mensaje.cantidad_bytes_total = strlen(this->broker_local.c_str());
	strcpy(mensaje.data, this->broker_local.c_str());
	mensaje.numero_de_mensaje = 0;

	grupo_remoto->push((char *) &mensaje,
			sizeof(mensajes::mensajes_local_broker_group_t));
	while (estoy_inicializando) {
		grupo_remoto->pull((char*) &mensaje,
				sizeof(mensajes::mensajes_local_broker_group_t));

		if (mensaje.tipo == mensajes::LEADER) { // soy el lider

			leader = true;
			if (!token_inicializado) {
				inicializar_grupo();
			}
			if (!mutex)
				launch_lider();
			estoy_inicializando = false;
		} else if (mensaje.tipo == mensajes::INITIALIZATION) {
			// no hay leader
			if (this->broker_local == mensaje.data) {
				//if (mensaje.numero_de_mensaje == 1) { // token ya inicializado
				token_inicializado = false;
				estoy_inicializando = false;
				//}
			}
		} else if (mensaje.tipo == mensajes::LEADER_ELECTED) {
			this->leader = false;
			this->token_inicializado = true;
			estoy_inicializando = false;
		} else {
			grupo_remoto->push((char *) &mensaje,
					sizeof(mensajes::mensajes_local_broker_group_t));
		}
	}
}

void GroupReceiver::launch_lider() {

	if (!mutex) {
		create_if_not_exists(
				std::string(directorio_de_trabajo).append(PATH_COLA_LEADER).c_str());
		strcpy(directorio_trabajo, directorio_de_trabajo.c_str());
		sprintf(id, "%d", id_grupo);
		strcpy(nombre_del_grupo, nombre_recurso.c_str());
		strcpy(nombre_broker_local, broker_local.c_str());
		this->leader_q =
				new MessageQueue(
						std::string(directorio_de_trabajo).append(
								PATH_COLA_LEADER).c_str(), char(0), 0664, true);
		this->numero_de_nodo = 1;
		//Process p("lider", args_lider);
	}
}

size_t GroupReceiver::recv_token() {
	bool mensaje_completo = false;
	bool token_roto = false;
	bool estoy_recibiendo_token = false;
   estoy_recibiendo_token = estoy_recibiendo_token;
	do {
		grupo_remoto->pull((char*) &mensaje,
				sizeof(mensajes::mensajes_local_broker_group_t));

		if (mensaje.tipo == mensajes::TOKEN_DELIVER) {
			std::cout << "Recibiendo Token " << mensaje.numero_de_mensaje
					<< std::endl;

			if (mensaje.numero_de_mensaje == 0) {
				numero_anterior = -1;
				token_roto = false;
				cantidad_recibida_token = 0;
				estoy_recibiendo_token = true;
			}
			if (!token_roto
					&& mensaje.numero_de_mensaje != numero_anterior + 1) {
				mensaje.tipo = mensajes::TOKEN_LOST;
				sprintf(mensaje.data, "%d", this->numero_de_nodo);
				grupo_remoto->push((char *) &mensaje,
						sizeof(mensajes::mensajes_local_broker_group_t));
				token_roto = true;
				cantidad_recibida_token += DATA_SIZE;
			} else if (!token_roto) {
				memcpy((data_token + mensaje.numero_de_mensaje * DATA_SIZE),
						mensaje.data,
						std::min(
								grupo.get_mem_size() - mensaje.numero_de_mensaje * DATA_SIZE, size_t(DATA_SIZE)));
								cantidad_recibida_token += DATA_SIZE;
								if (cantidad_recibida_token >= mensaje.cantidad_bytes_total) {
									mensaje_completo = true;
									tipo_mensaje_recibido = mensajes::TOKEN_DELIVER;
									cantidad_recibida_token = 0;
									numero_anterior = -1;
								} else {
									numero_anterior++;
								}
							}
			this->token_inicializado = true; // el token ya existe en el grupo
		} else if (mensaje.tipo == mensajes::TOKEN_IN_TRANSIT) {
			avisar_llegada_token = true;
		} else if (mensaje.tipo == mensajes::GROUP_DISCOVER) {
			memcpy(data_group, mensaje.data, DATA_SIZE);
			cantidad_recibida_grupo += DATA_SIZE;
			if (cantidad_recibida_grupo >= mensaje.cantidad_bytes_total) {
				mensaje_completo = true;
				tipo_mensaje_recibido = mensajes::GROUP_DISCOVER;
				cantidad_recibida_grupo = 0;
			}
		} else if (mensaje.tipo == mensajes::MEMORY_UPDATE) {
			std::cout << "Recibiendo Replica " << mensaje.numero_de_mensaje
					<< std::endl;

			if (mensaje.numero_de_mensaje == 0) {
				cantidad_recibida_replica = 0;
				replica_ok = false;
				numero_anterior_replica = -1;
			}
			if (mensaje.numero_de_mensaje != numero_anterior_replica + 1) {
				memcpy((data_replica + mensaje.numero_de_mensaje * DATA_SIZE),
						mensaje.data,
						std::min(
								grupo.get_mem_size() - mensaje.numero_de_mensaje * DATA_SIZE, size_t(DATA_SIZE)));
								cantidad_recibida_replica += DATA_SIZE;
								if (cantidad_recibida_replica >= mensaje.cantidad_bytes_total) {
									replica_ok = true;
									numero_anterior_replica = -1;
									memcpy(data_token, data_replica, grupo.get_mem_size());
								}
								numero_anterior_replica++;
								if (!leader) {
									forward_mensaje();
								}
							} else {
								cantidad_recibida_replica = 0;
								numero_anterior_replica = -1;
							}
						} else if (mensaje.tipo == mensajes::TOKEN_DISCOVER) {
							if(leader) {
								reenviar_mensaje_a_lider();
							} else {
								if(this->grupo.tengo_el_token()) {
									mensaje.tipo = mensajes::TOKEN_OK;
									sprintf(mensaje.data, "%d", this->numero_de_nodo);
									forward_mensaje();
								} else {
									/*if(this->grupo.esta_enviando_token()) {
									 mensaje.tipo = mensajes::TOKEN_IN_TRANSIT;
									 sprintf(mensaje.data, "%d", this->numero_de_nodo);
									 }
									 forward_mensaje();*/
									grupo.avisar_si_se_esta_enviando_token();
								}
							}
						} else {
							mensaje_completo = true;
						}
					} while (!mensaje_completo);

	return 0;
}

void GroupReceiver::inicializar_grupo() {
	char path[FILENAME_MAX];
	strcpy(path, directorio_de_trabajo.c_str());
	strcat(path, "/");
	strcat(path, nombre_recurso.c_str());
	strcat(path, POSTFIJO_INIT);
	grupo.inicializar_memoria(path);
	this->memoria_inicializada = true;
	if (crear_token) {
		grupo.release_token(&cola_token_manager);
	}
}

void GroupReceiver::reenviar_mensaje_a_lider() {
	mensajes::mensajes_local_broker_lider_t msg;
	if (!mutex) {
		msg.mtype = 1;
		msg.data = mensaje;
		//memcpy(msg.data, &mensaje,
		//		sizeof(mensajes::mensajes_local_broker_group_t));
		leader_q->push((char *) &msg,
				sizeof(mensajes::mensajes_local_broker_group_t));
		std::cout << "Msg to Leader: GROUP_DISCOVER" << std::endl;
	}
}

void GroupReceiver::actualizar_numero_de_nodo() {
	sscanf(mensaje.data, "%d", &this->numero_de_nodo);
	this->numero_de_nodo++;
	sprintf(mensaje.data, "%d", this->numero_de_nodo);
}

void GroupReceiver::forward_mensaje() {
	grupo_remoto->push((char *) &mensaje,
			sizeof(mensajes::mensajes_local_broker_group_t));
}

void GroupReceiver::procesar_mensaje() {
	if (mensaje.tipo == mensajes::TOKEN_DELIVER) {
		entregar_token_a_aplicacion();
		if (avisar_llegada_token) {
			mensaje.tipo = mensajes::TOKEN_OK;
			mensaje.cantidad_bytes_total = 1;
			mensaje.numero_de_mensaje = 0;
			strncpy(mensaje.data,"%d",this->numero_de_nodo);
			avisar_llegada_token = false;
		}
	} else if (mensaje.tipo == mensajes::GROUP_DISCOVER) {
		if (leader) {
			reenviar_mensaje_a_lider();
		} else {
			actualizar_numero_de_nodo();
			forward_mensaje();
		}
	} else if (mensaje.tipo == mensajes::INITIALIZATION) {
		if (this->broker_local != mensaje.data) {
			if (this->leader) {
				mensaje.tipo = mensajes::LEADER_ELECTED;
			}
			forward_mensaje();
		}
	} else if (mensaje.tipo == mensajes::LEADER && !this->leader) {
		this->leader = true;
		this->numero_de_nodo = 1;
		launch_lider();
	} else if (mensaje.tipo == mensajes::TOKEN_RECOVER) {
		if (leader) {
			reenviar_mensaje_a_lider();
		} else if (strcmp(mensaje.data, "OK") != 0) {
			int entero;
			sscanf(mensaje.data, "%d", &entero);
			if (entero == this->numero_de_nodo) {
				grupo.reenviar_token_al_cliente();
				strcpy(mensaje.data, "OK");
				grupo_remoto->push((char *) &mensaje,
						sizeof(mensajes::mensajes_local_broker_group_t));
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
	inicializando();
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

	chdir("local_broker");

	if (argc < 6) {
		std::cerr
				<< "Falta el directorio de trabajo, el id, el nombre del recurso, el nombre_broker_local, lanzar_token"
				<< std::endl;
		return -1;
	}
	id = atoi(argv[2]);

	GroupReceiver handler(argv[1], argv[3], id, argv[4],
			strcmp("1", argv[5]) == 0);

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
