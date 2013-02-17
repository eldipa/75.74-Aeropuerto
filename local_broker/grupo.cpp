/*
 * TablaClientes.cpp
 *
 *  Created on: 07/02/2013
 *      Author: gonzalo
 */

#include "grupo.h"
#include "local_broker_constants.h"
#include "genericerror.h"
#include "local_broker_messages.h"
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "init_parser.h"

Grupo::Grupo(const std::string & directorio_de_trabajo, std::string nombre_recurso)
	:
		memoria(
			std::string(directorio_de_trabajo).append(PREFIJO_RECURSO).append(nombre_recurso).append(".lck").c_str(),
			char(0), 0, false, false),
		mutex(std::string(directorio_de_trabajo).append(PREFIJO_RECURSO).append(nombre_recurso).append(".lck").c_str(),
			char(1), 0),
		cola(std::string(directorio_de_trabajo).append(PREFIJO_RECURSO).append(nombre_recurso).append(".lck").c_str(),
			char(2)), nombre_recurso(nombre_recurso)
{
	cant_clientes = static_cast<int *>(memoria.memory_pointer());
	mem_size = reinterpret_cast<size_t *>(cant_clientes + 1);
	client_names [0] = reinterpret_cast<char *>(mem_size + 1);
	for (int i = 1 ; i < MAX_CLIENTS ; i++) {
		client_names [i] = client_names [i - 1] + MAX_NOMBRE_RECURSO * sizeof(char);
	}
	memoria_compartida = reinterpret_cast<void *>(client_names [0] + MAX_CLIENTS * MAX_NOMBRE_RECURSO * sizeof(char));
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
			client_names [i] = client_names [i - 1] + MAX_NOMBRE_RECURSO * sizeof(char);
			(*client_names) [i] = '\0';
		}
		memoria_compartida = reinterpret_cast<void *>(client_names [0] + MAX_CLIENTS * MAX_NOMBRE_RECURSO * sizeof(char));

		*cant_clientes = 0;
		*mem_size = tamanio_memoria;

		// al primero le entrega el token
		traspaso_token_t mensaje;
		mensaje.mtype = 1; // el primero que entre al grupo se lleva el token
		strncpy(mensaje.recurso, nombre_recurso.c_str(), MAX_NOMBRE_RECURSO);
		cola.push(&mensaje, sizeof(traspaso_token_t) - sizeof(long));
	}
	cliente_actual = 0;
	token_owner = -1;
}

Grupo::~Grupo() {
}

void Grupo::join(const char nombre [MAX_NOMBRE_RECURSO]) {
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
			strncpy(client_names [i], nombre, MAX_NOMBRE_RECURSO);
			(*cant_clientes)++;
			numero_cola_asignada = i + 1;
			break;
		}
	}

	std::cout << "Join: " << nombre << " to " << this->nombre_recurso << std::endl;

	mutex.signalize(0);
}

void Grupo::leave(const char nombre [MAX_NOMBRE_RECURSO]) {
	mutex.wait_on(0);

	for (int i = 0 ; i < MAX_CLIENTS ; i++) {
		if (strncmp(client_names [i], nombre, MAX_NOMBRE_RECURSO) == 0) {
			*(client_names [i]) = '\0';
			(*cant_clientes)--;
			break;
		}
	}

	std::cout << "Leave: " << nombre << " from " << this->nombre_recurso << std::endl;

	mutex.signalize(0);
}

bool Grupo::locked_ya_esta_cliente(const char nombre [MAX_NOMBRE_RECURSO]) {
	for (int i = 0 ; i < MAX_CLIENTS ; i++) {
		if (strncmp(client_names [i], nombre, MAX_NOMBRE_RECURSO) == 0) {
			mutex.signalize(0);
			return true;
		}
	}
	return false;
}

bool Grupo::ya_esta_cliente(const char nombre [MAX_NOMBRE_RECURSO]) {
	bool value;
	mutex.wait_on(0);
	value = locked_ya_esta_cliente(nombre);
	mutex.signalize(0);
	return value;
}

unsigned short Grupo::obtener_numero_cola_de_cliente(const char nombre [MAX_NOMBRE_RECURSO]) {
	mutex.wait_on(0);

	for (unsigned short i = 0 ; i < MAX_CLIENTS ; i++) {
		if (strncmp(client_names [i], nombre, MAX_NOMBRE_RECURSO) == 0) {
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

	std::cout << "Passing Token from " << this->nombre_recurso << " to: " << client_names [cliente_actual] << "("
		<< cliente_actual << ")" << std::endl;

	cola.push(&mensaje, sizeof(traspaso_token_t) - sizeof(long));
	mutex.signalize(0);
}

void Grupo::lock_token() {

	cola.pull(&mensaje, sizeof(traspaso_token_t) - sizeof(long), this->numero_cola_asignada);
}

void Grupo::release_token(MessageQueue * cola_token_mgr) {
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

std::string Grupo::get_nombre_recurso() {
	return this->nombre_recurso;
}

void Grupo::inicializar_memoria(const std::string & init_file) {
	FILE * file;
	InitParser parser;
	char data [100];
	file = fopen(init_file.c_str(), "rt");
	void * p = this->memoria_compartida;

	if (file == NULL)
		throw GenericError("El archivo %s no existe", init_file.c_str());

	fscanf(file, "%*[^\n]\n"); // Evito la primera linea
	int * int_val;

	int vector;
	int i;

	while (fscanf(file, "%[^\n]\n", data) != EOF) {
		parser.parse_line(data);
		switch (parser.type()) {
			case T_INT:
				vector = parser.vector();
				int_val = (int *)p;
				i = 0;
				while(i < vector){
					(*int_val) = parser.int_val();
					int_val++;
					i++;
				}
				p = (void *)(int_val);
				break;
			default:
				break;
		}
	}

	fclose(file);
}
