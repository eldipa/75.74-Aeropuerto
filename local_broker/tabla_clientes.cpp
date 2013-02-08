/*
 * TablaClientes.cpp
 *
 *  Created on: 07/02/2013
 *      Author: gonzalo
 */

#include "tabla_clientes.h"
#include "local_broker_constants.h"
#include "genericerror.h"
#include <cstring>

TablaClientes::TablaClientes(const std::string & directorio_de_trabajo)
	: tabla(std::string(directorio_de_trabajo).append(PATH_TABLA_CLIENTES_LOCALES).c_str(), char(0), 0, false, false),
		mutex(std::string(directorio_de_trabajo).append(PATH_MUTEX_CLIENTES_LOCALES).c_str(), char(0), 1)
{
	cant_clientes = static_cast<int *>(tabla.memory_pointer());
	client_names = reinterpret_cast<char **>(cant_clientes + 1);
	colas_asignadas = reinterpret_cast<short unsigned int*>(client_names + MAX_CLIENTS * MAX_NAME_SIZE);
	cliente_actual = 0;
}

TablaClientes::TablaClientes(const std::string & directorio_de_trabajo, bool create)
	:
		// TODO
		tabla(std::string(directorio_de_trabajo).append(PATH_TABLA_CLIENTES_LOCALES).c_str(), char(0),
			TAMANIO_TABLA_CLIENTES, 0664, false, false),
		mutex(std::vector<short unsigned int>(1, 1),
			std::string(directorio_de_trabajo).append(PATH_MUTEX_CLIENTES_LOCALES).c_str(), char(0), 1)
{
	if (create) {
		cant_clientes = static_cast<int *>(tabla.memory_pointer());
		client_names = reinterpret_cast<char **>(cant_clientes + 1);
		colas_asignadas = reinterpret_cast<short unsigned int*>(client_names + MAX_CLIENTS * MAX_NAME_SIZE);

		*cant_clientes = 0;
		for (int i = 0 ; i < MAX_CLIENTS ; i++) {
			*(client_names [i]) = '\0';
			colas_asignadas [i] = 0;
		}
	}
	cliente_actual = 0;
}

TablaClientes::~TablaClientes() {
	// TODO Auto-generated destructor stub
}

void TablaClientes::agregar_cliente(char nombre [MAX_NAME_SIZE]) {
	mutex.wait_on(0);

	if (*cant_clientes == MAX_CLIENTS) {
		mutex.signalize(0);
		throw GenericError("Cantidad maxima(%d) de clientes alcanzada", MAX_CLIENTS);
	}
	if (ya_esta_cliente(nombre)) {
		mutex.signalize(0);
		throw GenericError("El cliente %d ya está registrado", nombre);
	}
	for (int i = 0 ; i < MAX_CLIENTS ; i++) {
		if (*(client_names [i]) == '\0') {
			strncpy(client_names [i], nombre, MAX_NAME_SIZE);
			(*cant_clientes)++;
			break;
		}
	}

	mutex.signalize(0);
}

void TablaClientes::remover_cliente(char nombre [MAX_NAME_SIZE]) {
	mutex.wait_on(0);

	for (int i = 0 ; i < MAX_CLIENTS ; i++) {
		if (strncmp(client_names [i], nombre, MAX_NAME_SIZE) == 0) {
			*(client_names [i]) = '\0';
			(*cant_clientes)--;
			break;
		}
	}

	mutex.signalize(0);
}

bool TablaClientes::ya_esta_cliente(char nombre [MAX_NAME_SIZE]) {
	mutex.wait_on(0);

	for (int i = 0 ; i < MAX_CLIENTS ; i++) {
		if (strncmp(client_names [i], nombre, MAX_NAME_SIZE) == 0) {
			mutex.signalize(0);
			return true;
		}
	}

	mutex.signalize(0);
	return false;
}

unsigned short TablaClientes::obtener_cola_de_cliente(char nombre [MAX_NAME_SIZE]) {
	mutex.wait_on(0);

	for (unsigned short i = 0 ; i < MAX_CLIENTS ; i++) {
		if (strncmp(client_names [i], nombre, MAX_NAME_SIZE) == 0) {
			mutex.signalize(0);
			return i;
		}
	}

	mutex.signalize(0);
	return 0;
}

unsigned short TablaClientes::obtener_proximo_cliente() {
	mutex.wait_on(0);

	if (*cant_clientes == 0) {
		mutex.signalize(0);
		return 0;
	}

	unsigned short i;

	i = (cliente_actual + 1) % MAX_CLIENTS;

	do {
		if (*(client_names [i]) != '\0') {
			cliente_actual = i;
			break;
		}
		i = (i + 1) % MAX_CLIENTS;
	} while (cliente_actual != i);

	mutex.signalize(0);
	return cliente_actual;
}
