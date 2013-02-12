/*
 * TablaClientes.h
 *
 *  Created on: 07/02/2013
 *      Author: gonzalo
 */

#ifndef TABLACLIENTES_H_
#define TABLACLIENTES_H_

#include "sharedmemory.h"
#include "semaphoreset.h"
#include "messagequeue.h"

#include "local_broker_messages.h"
#include "local_broker_constants.h"

class Grupo {
private:

	SharedMemory memoria;
	SemaphoreSet mutex;
	MessageQueue cola;

	long numero_cola_asignada;

	int * cant_clientes;
	size_t * mem_size;
	char * client_names [MAX_CLIENTS];
	unsigned short * colas_asignadas;
	int * id;

	long token_owner;

	unsigned short cliente_actual;

	std::string nombre_recurso;

	void * memoria_compartida;

	bool locked_ya_esta_cliente(const char nombre [MAX_NAME_SIZE]);

public:
	Grupo(const std::string & directorio_de_trabajo, std::string nombre_recurso);
	Grupo(const std::string & directorio_de_trabajo, std::string nombre_recurso, size_t tamanio_memoria, bool create);
	virtual ~Grupo();

	void join(const char nombre [MAX_NAME_SIZE]);
	void leave(const char nombre [MAX_NAME_SIZE]);
	bool ya_esta_cliente(const char nombre [MAX_NAME_SIZE]);

	unsigned short obtener_numero_cola_de_cliente(const char nombre [MAX_NAME_SIZE]);
	unsigned short obtener_proximo_cliente();

	void pasar_token_a_proximo_cliente();

	void lock_token();
	void release_token(MessageQueue *);

	size_t get_mem_size();
	void* memory_pointer();

};

#endif /* TABLACLIENTES_H_ */
