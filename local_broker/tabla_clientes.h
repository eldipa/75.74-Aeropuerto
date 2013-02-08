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

#define MAX_NAME_SIZE 30
#define MAX_CLIENTS 20
#define TAMANIO_TABLA_CLIENTES (sizeof(int) + sizeof(char) * MAX_NAME_SIZE * MAX_CLIENTS)

class TablaClientes {
private:

	SharedMemory tabla;
	SemaphoreSet mutex;

	int * cant_clientes;
	char ** client_names;
	unsigned short * colas_asignadas;

	unsigned short cliente_actual;

public:
	TablaClientes(const std::string & directorio_de_trabajo);
	TablaClientes(const std::string & directorio_de_trabajo, bool create);
	virtual ~TablaClientes();

	void agregar_cliente(char nombre [MAX_NAME_SIZE]);
	void remover_cliente(char nombre [MAX_NAME_SIZE]);
	bool ya_esta_cliente(char nombre [MAX_NAME_SIZE]);

	unsigned short obtener_cola_de_cliente(char nombre [MAX_NAME_SIZE]);
	unsigned short obtener_proximo_cliente();
};

#endif /* TABLACLIENTES_H_ */
