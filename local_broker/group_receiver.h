/*
 * GroupReceiver.h
 *
 *  Created on: 17/02/2013
 *      Author: gonzalo
 */

#ifndef GROUPRECEIVER_H_
#define GROUPRECEIVER_H_

#include "local_broker_constants.h"
#include "messagequeue.h"
#include "grupo.h"
#include "group_interface.h"
#include "mensajes_de_red.h"
#include <string>

class GroupReceiver {
private:
	MessageQueue cola_token_manager;
	MessageQueue * leader_q;
	GroupInterface * grupo_remoto;
	Grupo grupo;

	mensajes::mensajes_local_broker_group_t mensaje;
	char data_group [DATA_SIZE];
	char * data_token;
	char * data_replica;

	size_t cantidad_de_bloques_por_token;
	std::string directorio_de_trabajo;
	std::string broker_local;
	std::string nombre_recurso;
	char id_grupo;

	size_t cantidad_recibida_token;
	size_t cantidad_recibida_grupo;
	size_t cantidad_recibida_replica;
	mensajes::GROUP_MSG_T tipo_mensaje_recibido;

	int numero_anterior;
	int numero_anterior_replica;
	bool mutex;

	bool leader;
	bool crear_token;
	bool token_inicializado;
	bool memoria_inicializada;
	bool replica_ok;
	bool avisar_llegada_token;

	int numero_de_nodo;

	void inicializar_grupo();
	void forward_mensaje();
	void entregar_token_a_aplicacion();
	void reenviar_mensaje_a_lider();
	void actualizar_numero_de_nodo();
	void inicializando();
	void loop_token();
	void launch_lider();

	size_t recv_token();
	void procesar_mensaje();

public:
	GroupReceiver(const std::string & directorio_de_trabajo, const std::string & nombre_grupo, char id,
		const std::string & nombre_broker_remoto, bool lanzar_token);
	virtual ~GroupReceiver();

	void run();
};

#endif /* GROUPRECEIVER_H_ */
