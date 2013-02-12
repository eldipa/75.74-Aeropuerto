/*
 * TablaClientes.cpp
 *
 *  Created on: 07/02/2013
 *      Author: gonzalo
 */

#include "grupo.h"
#include "local_broker_constants.h"
#include "genericerror.h"
#include <cstring>
#include "local_broker_messages.h"

Grupo::Grupo(const std::string & directorio_de_trabajo, std::string nombre_recurso)
	:
		memoria(
			std::string(directorio_de_trabajo).append(PREFIJO_RECURSO).append(nombre_recurso).append(".lck").c_str(),
			char(0), 0, false, false),
		mutex(std::string(directorio_de_trabajo).append(PREFIJO_RECURSO).append(nombre_recurso).append(".lck").c_str(),
			char(1), 1),
		cola(std::string(directorio_de_trabajo).append(PREFIJO_RECURSO).append(nombre_recurso).append(".lck").c_str(),
			char(2)), nombre_recurso(nombre_recurso)
{
	cant_clientes = static_cast<int *>(memoria.memory_pointer());
	mem_size = reinterpret_cast<size_t *>(cant_clientes + 1);
	client_names [0] = reinterpret_cast<char *>(mem_size + 1);
	for (int i = 1 ; i < MAX_CLIENTS ; i++) {
		client_names [i] = client_names [i - 1] + MAX_NAME_SIZE * sizeof(char);
	}
	memoria_compartida = reinterpret_cast<void *>(client_names [0] + MAX_CLIENTS * MAX_NAME_SIZE * sizeof(char));
}

Grupo::Grupo(const std::string & directorio_de_trabajo, std::string nombre_recurso, size_t tamanio_memoria, bool create)
	:
		memoria(
			std::string(directorio_de_trabajo).append(PREFIJO_RECURSO).append(nombre_recurso).append(".lck").c_str(),
			char(0), TAMANIO_TABLA_CLIENTES + tamanio_memoria, 0664, true, false),
		mutex(std::vector<short unsigned int>(1, 1),
			std::string(directorio_de_trabajo).append(PREFIJO_RECURSO).append(nombre_recurso).append(".lck").c_str(),
			char(1), 0664),
		cola(std::string(directorio_de_trabajo).append(PREFIJO_RECURSO).append(nombre_recurso).append(".lck").c_str(),
			char(2), 0664, true), nombre_recurso(nombre_recurso)
{
	if (create) {
		cant_clientes = static_cast<int *>(memoria.memory_pointer());
		mem_size = reinterpret_cast<size_t *>(cant_clientes + 1);
		client_names [0] = reinterpret_cast<char *>(mem_size + 1);
		for (int i = 1 ; i < MAX_CLIENTS ; i++) {
			client_names [i] = client_names [i - 1] + MAX_NAME_SIZE * sizeof(char);
			(*client_names) [i] = '\0';
			//colas_asignadas [i] = 0;
		}
		memoria_compartida = reinterpret_cast<void *>(client_names [0] + MAX_CLIENTS * MAX_NAME_SIZE * sizeof(char));

		*cant_clientes = 0;
		*mem_size = tamanio_memoria;

		// al primero le entrega el token
		traspaso_token_t mensaje;
		mensaje.mtype = 1; // el primero que entre al grupo se lleva el token
		strncpy(mensaje.recurso, nombre_recurso.c_str(), MAX_NAME_SIZE);
		cola.push(&mensaje, sizeof(traspaso_token_t) - sizeof(long));
	}
	cliente_actual = 0;
	token_owner = -1;
}

Grupo::~Grupo() {
}

void Grupo::join(const char nombre [MAX_NAME_SIZE]) {
	mutex.wait_on(0);

	if (*cant_clientes == MAX_CLIENTS) {
		mutex.signalize(0);
		throw GenericError("Cantidad maxima(%d) de clientes alcanzada", MAX_CLIENTS);
	}
	if (locked_ya_esta_cliente(nombre)) {
		mutex.signalize(0);
		throw GenericError("El cliente %d ya está registrado", nombre);
	}
	for (int i = 0 ; i < MAX_CLIENTS ; i++) {
		if (*(client_names [i]) == '\0') {
			strncpy(client_names [i], nombre, MAX_NAME_SIZE);
			(*cant_clientes)++;
			numero_cola_asignada = i + 1;
			break;
		}
	}

	mutex.signalize(0);
}

void Grupo::leave(const char nombre [MAX_NAME_SIZE]) {
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

bool Grupo::locked_ya_esta_cliente(const char nombre [MAX_NAME_SIZE]) {
	for (int i = 0 ; i < MAX_CLIENTS ; i++) {
		if (strncmp(client_names [i], nombre, MAX_NAME_SIZE) == 0) {
			mutex.signalize(0);
			return true;
		}
	}
	return false;
}

bool Grupo::ya_esta_cliente(const char nombre [MAX_NAME_SIZE]) {
	bool value;
	mutex.wait_on(0);
	value = locked_ya_esta_cliente(nombre);
	mutex.signalize(0);
	return value;
}

unsigned short Grupo::obtener_numero_cola_de_cliente(const char nombre [MAX_NAME_SIZE]) {
	mutex.wait_on(0);

	for (unsigned short i = 0 ; i < MAX_CLIENTS ; i++) {
		if (strncmp(client_names [i], nombre, MAX_NAME_SIZE) == 0) {
			mutex.signalize(0);
			return i + 1;
		}
	}

	mutex.signalize(0);
	return 0;
}

unsigned short Grupo::obtener_proximo_cliente() {
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

void Grupo::pasar_token_a_proximo_cliente() {
	int i;
	mutex.wait_on(0);
	traspaso_token_t mensaje;

	if (*cant_clientes == 0) {
		mensaje.mtype = 1;
		cliente_actual = 0;
	} else {
		i = (cliente_actual + 1) % MAX_CLIENTS;
		do {
			if (*(client_names [i]) != '\0') {
				cliente_actual = i;
				break;
			}
			i = (i + 1) % MAX_CLIENTS;
		} while (cliente_actual != i);

		mensaje.mtype = cliente_actual + 1;
	}
	cola.push(&mensaje, sizeof(traspaso_token_t) - sizeof(long));
	mutex.signalize(0);
}

void Grupo::lock_token() {
	traspaso_token_t mensaje;

	cola.pull(&mensaje, sizeof(traspaso_token_t) - sizeof(long), this->numero_cola_asignada);
}

void Grupo::release_token(MessageQueue * cola_token_mgr) {
	traspaso_token_t mensaje;
	mensaje.mtype = 1;
	strncpy(mensaje.recurso, this->nombre_recurso.c_str(), sizeof(mensaje.recurso));

	cola_token_mgr->push(&mensaje, sizeof(traspaso_token_t) - sizeof(long));
}

size_t Grupo::get_mem_size() {
	return *this->mem_size;
}

void* Grupo::memory_pointer() {
	return memoria_compartida;
}

std::string Grupo::get_nombre_recurso(){
	return this->nombre_recurso;
}
