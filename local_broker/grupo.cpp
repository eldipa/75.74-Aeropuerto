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
	: memoria(std::string(directorio_de_trabajo).append(PATH_TABLA_GRUPOS).c_str(), char(0), 0, false, false),
		semaforos(std::string(directorio_de_trabajo).append(PATH_TABLA_GRUPOS).c_str(), char(1), 1),
		cola(std::string(directorio_de_trabajo).append(PATH_TABLA_GRUPOS).c_str(), char(2)),
		nombre_recurso(nombre_recurso)
{
	bool creado = false;
	mensaje.tipo = 1;
	int posicion_grupo;

	asignar_punteros_control();
	while (!creado) {
		semaforos.wait_on(0);
		for (int i = 0 ; i < MAX_GRUPOS ; i++) {
			if (*grupos_creados [i] != '\0'
				&& strncmp(nombre_recurso.c_str(), grupos_creados [i], MAX_NOMBRE_RECURSO) == 0)
			{
				posicion_grupo = i;
				creado = true;
				break;
			}
		}
		if (creado) {
			semaforos.signalize(0);
		} else {
			(*cantidad_clientes_esperando)++;
			semaforos.signalize(0);
			semaforos.wait_on(1);
		}
	}

	asignar_punteros(static_cast<char *>(memoria.memory_pointer()) + offsets [posicion_grupo]);
	mutex_asignado = posicion_grupo + 2;
	numero_base_cola_asignada = posicion_grupo * MAX_CLIENTS + 1;

}

Grupo::Grupo(const std::string & directorio_de_trabajo, std::string nombre_recurso, size_t tamanio_memoria, bool create)
	: memoria(std::string(directorio_de_trabajo).append(PATH_TABLA_GRUPOS).c_str(), char(0), 0, false, false),
		semaforos(std::string(directorio_de_trabajo).append(PATH_TABLA_GRUPOS).c_str(), char(1), 1),
		cola(std::string(directorio_de_trabajo).append(PATH_TABLA_GRUPOS).c_str(), char(2)),
		nombre_recurso(nombre_recurso)
{
	bool creado = false;
	mensaje.tipo = 1;
	int posicion_grupo;

	if (create) {
		asignar_punteros_control();

		semaforos.wait_on(0);

		for (int i = 0 ; i < MAX_GRUPOS ; i++) {
			if (*grupos_creados [i] != '\0'
				&& strncmp(nombre_recurso.c_str(), grupos_creados [i], MAX_NOMBRE_RECURSO) == 0)
			{
				creado = true;
				posicion_grupo = i;
				break;
			}
			if (!creado && *grupos_creados [i] == '\0') {
				strncpy(grupos_creados [i], nombre_recurso.c_str(), MAX_NOMBRE_RECURSO);
				posicion_grupo = i;
				break;
			}
		}
		// levanto los clientes que esperan
		for (int i = 0 ; i < *cantidad_clientes_esperando ; i++) {
			semaforos.signalize(1);
		}
		*cantidad_clientes_esperando = 0;

		if (posicion_grupo < MAX_GRUPOS) {
			offsets [posicion_grupo + 1] = offsets [posicion_grupo] + tamanio_memoria + TAMANIO_TABLA_CLIENTES;
		}

		asignar_punteros(static_cast<char *>(memoria.memory_pointer()) + offsets [posicion_grupo]);

		mutex_asignado = posicion_grupo + 2;
		numero_base_cola_asignada = posicion_grupo * MAX_CLIENTS + 1;

		for (int i = 1 ; i < MAX_CLIENTS ; i++) {
			(*client_names) [i] = '\0';
		}

		*cant_clientes = 0;
		*mem_size = tamanio_memoria;
		*avisar_envio = 0;

		*tengo_token = 0;
		cliente_actual = 0;
		token_owner = -1;
		*token_enviado = 1;

		semaforos.signalize(0);

	}
}

Grupo::Grupo(const std::string & directorio_de_trabajo, size_t tamanio_total, size_t cantidad_de_grupos)
	:
		memoria(std::string(directorio_de_trabajo).append(PATH_TABLA_GRUPOS).c_str(), char(0),
			tamanio_total + TAMANIO_TABLA_GRUPOS_CREADOS, 0664, true, false),
		semaforos(std::vector<short unsigned int>(cantidad_de_grupos + 2, 1),
			std::string(directorio_de_trabajo).append(PATH_TABLA_GRUPOS).c_str(), char(1), 0664),
		cola(std::string(directorio_de_trabajo).append(PATH_TABLA_GRUPOS).c_str(), char(2), 0664, true)
{
	asignar_punteros_control();

	for (int i = 1 ; i < MAX_GRUPOS ; i++) {
		*grupos_creados [i] = '\0';
	}
	offsets [0] = TAMANIO_TABLA_GRUPOS_CREADOS;
	semaforos.wait_on(1); // ajusto el valor del semaforo
}

Grupo::~Grupo() {
}

void Grupo::asignar_punteros(void * p) {
	cant_clientes = reinterpret_cast<int *>(p);
	mem_size = reinterpret_cast<size_t *>(cant_clientes + 1);
	client_names [0] = reinterpret_cast<char *>(mem_size + 1);
	for (int i = 1 ; i < MAX_CLIENTS ; i++) {
		client_names [i] = client_names [i - 1] + MAX_NOMBRE_RECURSO * sizeof(char);
	}
	tengo_token = reinterpret_cast<int *>(client_names [0] + MAX_CLIENTS * MAX_NOMBRE_RECURSO * sizeof(char));

	token_enviado = tengo_token + 1;
	avisar_envio = token_enviado + 1;
	memoria_compartida = reinterpret_cast<void *>(avisar_envio + 1);
}

void Grupo::asignar_punteros_control() {
	cantidad_clientes_esperando = static_cast<int *>(memoria.memory_pointer());

	grupos_creados [0] = reinterpret_cast<char *>(cantidad_clientes_esperando + 1);

	for (int i = 1 ; i < MAX_GRUPOS ; i++) {
		grupos_creados [i] = grupos_creados [i - 1] + sizeof(char) * MAX_NOMBRE_RECURSO;
	}

	offsets = reinterpret_cast<size_t *>(grupos_creados [MAX_GRUPOS - 1] + sizeof(char) * MAX_NOMBRE_RECURSO);
}

void Grupo::join(const char nombre [MAX_NOMBRE_RECURSO]) {
	semaforos.wait_on(mutex_asignado);

	if (*cant_clientes == MAX_CLIENTS) {
		semaforos.signalize(mutex_asignado);
		throw GenericError("Cantidad maxima(%d) de clientes alcanzada", MAX_CLIENTS);
	}
	if (locked_ya_esta_cliente(nombre)) {
		semaforos.signalize(mutex_asignado);
		throw GenericError("El cliente %s ya esta registrado en %s", nombre, this->nombre_recurso.c_str());
	}
	for (int i = 0 ; i < MAX_CLIENTS ; i++) {
		if (*(client_names [i]) == '\0') {
			strncpy(client_names [i], nombre, MAX_NOMBRE_RECURSO);
			(*cant_clientes)++;
			numero_cola_asignada = i + numero_base_cola_asignada;
			break;
		}
	}

	Log::info("Joined: %s to %s\n", nombre, this->nombre_recurso.c_str());
	//std::cout << "Joined: " << nombre << " to " << this->nombre_recurso << std::endl;

	semaforos.signalize(mutex_asignado);
}

bool Grupo::existe_grupo(const char nombre_grupo [MAX_NOMBRE_RECURSO]) {
	bool existe = false;
	semaforos.wait_on(0);

	for (int i = 0 ; i < MAX_GRUPOS ; i++) {
		if (*grupos_creados [i] != '\0' && strncmp(nombre_grupo, grupos_creados [i], MAX_NOMBRE_RECURSO) == 0) {
			existe = true;
			break;
		}
	}

	semaforos.signalize(0);
	return existe;
}

void Grupo::leave(const char nombre [MAX_NOMBRE_RECURSO]) {
	semaforos.wait_on(mutex_asignado);

	for (int i = 0 ; i < MAX_CLIENTS ; i++) {
		if (strncmp(client_names [i], nombre, MAX_NOMBRE_RECURSO) == 0) {
			*(client_names [i]) = '\0';
			(*cant_clientes)--;
			break;
		}
	}
	Log::info("Leave: %s from %s\n", nombre, this->nombre_recurso.c_str());
	//std::cout << "Leave: " << nombre << " from " << this->nombre_recurso << std::endl;

	semaforos.signalize(mutex_asignado);
}

bool Grupo::locked_ya_esta_cliente(const char nombre [MAX_NOMBRE_RECURSO]) {
	for (int i = 0 ; i < MAX_CLIENTS ; i++) {
		if (strncmp(client_names [i], nombre, MAX_NOMBRE_RECURSO) == 0) {
			semaforos.signalize(mutex_asignado);
			return true;
		}
	}
	return false;
}

bool Grupo::ya_esta_cliente(const char nombre [MAX_NOMBRE_RECURSO]) {
	bool value;
	semaforos.wait_on(mutex_asignado);
	value = locked_ya_esta_cliente(nombre);
	semaforos.signalize(mutex_asignado);
	return value;
}

unsigned short Grupo::obtener_proximo_cliente() {
	semaforos.wait_on(mutex_asignado);
	mensaje.tipo = 1;

	if (*cant_clientes == 0) {
		semaforos.signalize(mutex_asignado);
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

	semaforos.signalize(mutex_asignado);
	return cliente_actual;
}

void Grupo::pasar_token_a_proximo_cliente() {
	int i;
	semaforos.wait_on(mutex_asignado);
	mensaje.tipo = 1;

	if (*cant_clientes == 0) {
		mensaje.mtype = numero_base_cola_asignada;
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

		mensaje.mtype = cliente_actual + numero_base_cola_asignada;
	}

	if (strncmp(client_names [cliente_actual], "localbroker", strlen("localbroker")) == 0) {
		*tengo_token = 0;
		*token_enviado = 0;
	} else {
		*tengo_token = 1;
	}

	//if (this->nombre_recurso == "cinta_principal" || this->nombre_recurso.substr(0, 4) == "cpp_") {
	//std::cout << "Passing Token from " << this->nombre_recurso.c_str() << " to: " << client_names [cliente_actual]
	//	<< " (" << cliente_actual << ")\n";

	//Log::debug("Passing Token from %s to: %s (%d)\n", this->nombre_recurso.c_str(), client_names [cliente_actual],
	//	cliente_actual);

	//}

	cola.push(&mensaje, sizeof(traspaso_token_t) - sizeof(long));
	semaforos.signalize(mutex_asignado);
}

// SOLO SE DEBE USAR CON group_sender
void Grupo::reenviar_token_al_cliente() {
	semaforos.wait_on(mutex_asignado);

	mensaje.mtype = cliente_actual + numero_base_cola_asignada;

	mensaje.tipo = 1;
	Log::debug("Resending Token from %s to: %s (%d)\n", this->nombre_recurso.c_str(), client_names [cliente_actual],
		cliente_actual);

	cola.push(&mensaje, sizeof(traspaso_token_t) - sizeof(long));
	semaforos.signalize(mutex_asignado);
}

int Grupo::lock_token() {

	cola.pull(&mensaje, sizeof(traspaso_token_t) - sizeof(long), this->numero_cola_asignada);
	return mensaje.tipo;
}

void Grupo::release_token(MessageQueue * cola_token_mgr) {
	mensaje.mtype = 1;
	mensaje.tipo = 1;
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

	if (*this->mem_size == 0) {
		return;
	}

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
				while (i < vector) {
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

bool Grupo::tengo_el_token() {
	bool result;
	semaforos.wait_on(mutex_asignado);
	result = (*tengo_token) == 1;
	semaforos.signalize(mutex_asignado);
	return result;
}

void Grupo::replicar_brokers() {

	if (*this->mem_size == 0) {
		return;
	}
	semaforos.wait_on(mutex_asignado);
	int j;
	bool encontrado = false;

	if (*cant_clientes > 0) {
		mensaje.mtype = numero_base_cola_asignada;
		mensaje.tipo = 2;

		for (j = 0; j < MAX_CLIENTS ; j++) {
			if (strncmp("localbroker", client_names [j], strlen("localbroker")) == 0) {
				encontrado = true;
				break;
			}
		}

		mensaje.mtype = j + numero_base_cola_asignada;

		if (encontrado) {
			//if (this->nombre_recurso == "cinta_principal" || this->nombre_recurso.substr(0, 4) == "cpp_") {
			//Log::debug("Replicating %s to: %s (%d)\n", this->nombre_recurso.c_str(), client_names [j], cliente_actual);
			//}

			cola.push(&mensaje, sizeof(traspaso_token_t) - sizeof(long));
		}
	}

	semaforos.signalize(mutex_asignado);
}

bool Grupo::esta_enviando_token() {
	bool result;
	semaforos.wait_on(mutex_asignado);
	result = ((*token_enviado) == 0);
	semaforos.signalize(mutex_asignado);
	return result;
}

void Grupo::el_token_se_envio() {
	semaforos.wait_on(mutex_asignado);
	(*token_enviado) = 1;
	(*avisar_envio) = 0;
	semaforos.signalize(mutex_asignado);
}

void Grupo::avisar_si_se_esta_enviando_token() {
	semaforos.wait_on(mutex_asignado);
	if (*tengo_token == 0 && *token_enviado == 0) {
		(*avisar_envio) = 1;
	}
	semaforos.signalize(mutex_asignado);
}

bool Grupo::tengo_que_avisar_envio() {
	bool result;
	semaforos.wait_on(mutex_asignado);
	result = ((*avisar_envio) == 1);
	semaforos.signalize(mutex_asignado);
	return result;
}
