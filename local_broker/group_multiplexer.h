/*
 * GroupMultiplexer.h
 *
 *  Created on: 03/03/2013
 *      Author: gonzalo
 */

#ifndef GROUPMULTIPLEXER_H_
#define GROUPMULTIPLEXER_H_

#include "group_interface.h"
#include "yasper.h"
#include "mensajes_de_red.h"
#include "sharedmemory.h"
#include "semaphoreset.h"
#include "messagequeue.h"
#include "local_broker_constants.h"
#include <map>
#include <string>

class GroupMultiplexer {
private:
	long numero_cola_asignada;
	GroupInterface  interfaz_grupos;
	MessageQueue  cola_espera_grupos;
	SharedMemory  memoria_grupos;
	SemaphoreSet  mutex;

	int * lider;
	char * grupos_creados [MAX_GRUPOS];

	std::map<std::string, long> grupos;

public:
	GroupMultiplexer(const char *absolute_path);
	GroupMultiplexer(const char *absolute_path, const char * nombre_grupo);
	virtual ~GroupMultiplexer();

	ssize_t pull(mensajes::mensajes_local_broker_group_t & mensaje);
	void push(mensajes::mensajes_local_broker_group_t & mensaje);

	void forward_message();
};

#endif /* GROUPMULTIPLEXER_H_ */
