/*
 * token_manager.cpp
 *
 *  Created on: 10/02/2013
 *      Author: gonzalo
 */

#include "token_manager.h"
#include "local_broker_constants.h"
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include "log.h"
#include "oserror.h"
#include "genericerror.h"
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <cstring>
#include "daemon.h"
#include "group_comm_manager.h"
#include "init_parser.h"

TokenManager::TokenManager(const std::string & directorio, const std::string & groups_file)
	: clientes(std::string(directorio).append(PATH_COLA_TOKEN_MANAGER).c_str(), char(0), 0664, true),
		memoria_grupos(std::string(directorio).append(PATH_COLA_TOKEN_MANAGER).c_str(), char(1),
			TAMANIO_TABLA_GRUPOS_CREADOS, 0664, true, false),
		semaforo_grupos(std::vector<short unsigned int>(2, 1),
			std::string(directorio).append(PATH_COLA_TOKEN_MANAGER).c_str(), char(2), 0664),
		directorio_de_trabajo(directorio), groups_file(groups_file), manager(directorio_de_trabajo)
{
	//crear_grupos(directorio_de_trabajo, groups_file);

	cantidad_clientes_esperando = static_cast<int *>(memoria_grupos.memory_pointer());

	grupos_creados [0] = reinterpret_cast<char *>(cantidad_clientes_esperando + 1);

	for (int i = 1 ; i < MAX_GRUPOS ; i++) {
		grupos_creados [i] = grupos_creados [i - 1] + sizeof(char) * MAX_NOMBRE_RECURSO;
		*grupos_creados [i] = '\0';
	}

	// ajusto el valor del semaforo 1
	semaforo_grupos.wait_on(1);

	manager.levantar_servicio();
}

TokenManager::~TokenManager() {
	Grupo * g;
	std::map<std::string, Grupo *>::iterator i;

	for (i = this->grupos.begin(); i != this->grupos.end() ; i++) {
		g = i->second;
		delete g;
	}
	manager.bajar_servicio();
}

void TokenManager::agregar_grupo_a_tabla_creados(const char nombre_grupo [MAX_NOMBRE_RECURSO]) {
	bool creado = false;
	semaforo_grupos.wait_on(0);
	for (int i = 0 ; i < MAX_GRUPOS ; i++) {
		if (*grupos_creados [i] != '\0' && strncmp(nombre_grupo, grupos_creados [i], MAX_NOMBRE_RECURSO) == 0) {
			creado = true;
			break;
		}
		if (!creado && *grupos_creados [i] == '\0') {
			strncpy(grupos_creados [i], nombre_grupo, MAX_NOMBRE_RECURSO);
			break;
		}
	}
	for (int i = 0 ; i < *cantidad_clientes_esperando ; i++) {
		semaforo_grupos.signalize(1);
	}
	*cantidad_clientes_esperando = 0;
	semaforo_grupos.signalize(0);
}

void TokenManager::crear_grupo(const std::string & directorio_de_trabajo, const std::string & groups_file,
	const std::string & group_name)
{
	FILE * f;
	char nombre_recurso [MAX_NOMBRE_RECURSO];
	char primera_linea [200];
	//int tamanio_memoria;
	char tamanio_memoria_str [200];
	int id_grupo;
	int valor;

	f = fopen(groups_file.c_str(), "rt");

	if (f == NULL) {
		throw OSError("No se pudieron crear los grupos: %s", groups_file.c_str());
	}
	// evito la primera linea
	fscanf(f, "%s\n", primera_linea);

	//manager.levantar_servicio();

	while (fscanf(f, "%[^:]:%[^:]:%d:%d\n", nombre_recurso, tamanio_memoria_str, &valor, &id_grupo) != EOF) {
		if (group_name == nombre_recurso) {
			// Crear Grupo
			Grupo * g;
			// Creo el archivo lck
			create_if_not_exists(
				std::string(directorio_de_trabajo).append(PREFIJO_RECURSO).append(nombre_recurso).append(POSTFIJO_LCK).c_str());
			g = new Grupo(directorio_de_trabajo, nombre_recurso, InitParser::parse_int_val(tamanio_memoria_str), true);
			grupos.insert(std::pair<std::string, Grupo *>(std::string(nombre_recurso), g));
			agregar_grupo_a_tabla_creados(group_name.c_str());

			//manager.levantar_grupo(nombre_recurso, char(id_grupo), valor);
			break;

			// DEBUG
			if (valor == 1) {
				// MAL LO TIENE QUE INICIALIZAR EL PROCESO "GROUP_RECEIVER" QUE ES EL QUE MANEJA AL LIDER
				//g->release_token(&clientes);
			}

			/*if (InitParser::parse_int_val(tamanio_memoria_str)) {
			 int pos;
			 char path [FILENAME_MAX];
			 strcpy(path, directorio_de_trabajo.c_str());
			 strcat(path, "/");
			 strcat(path, nombre_recurso);
			 pos = strlen(path) - 1;
			 while (path [pos] <= '9' && path [pos] >= '0') {
			 path [pos--] = '\0';
			 }
			 strcat(path, POSTFIJO_INIT);
			 g->inicializar_memoria(path);
			 }*/
			// DEBUG
		}
	}
	fclose(f);
}

void TokenManager::run() {
	bool salir = false;
	traspaso_token_t mensaje;
	Grupo * g;
	std::string recurso;
	//traspaso_vista_t vista;

	// PARA DEBUG
	//strncpy((char *) g->memory_pointer(),"token_manager:1",512);
	//g = grupos.at("cinta_principal");
	/*for (int i = 0 ; i < 512 ; i++) {
	 ((char *)g->memory_pointer()) [i] = 'A';
	 }
	 for (int i = 512 ; i < 1023 ; i++) {
	 ((char *)g->memory_pointer()) [i] = 'B';
	 }
	 ((char *)g->memory_pointer()) [1023] = '\0';
	 std::cout << (char *)g->memory_pointer() << std::endl;*/

	ignore_signals();
	//char data [30];

	//std::cout << data << std::endl;
	do {
		try {
			// Recibo todos los tokens
			clientes.pull(&mensaje, sizeof(traspaso_token_t) - sizeof(long), 0);
			//Me fijo que token me dieron
			recurso.assign(mensaje.recurso);
			if (mensaje.tipo == 3) {
				this->crear_grupo(this->directorio_de_trabajo, this->groups_file, recurso);
			} else {
				if (grupos.count(recurso) < 1) {
					throw GenericError("Error Grupo %s no encontrado", mensaje.recurso);
				}
				g = grupos.at(recurso);
				//sleep(1);

				g->replicar_brokers();
				g->pasar_token_a_proximo_cliente();
			}
			usleep(300000);
		} catch (OSError & error) {
			//std::cerr << error.what() << std::endl;
			salir = true;
		}
	} while (!salir);

}

int main(int argc, char * argv [])
try
{
	if (argc != 4) {
		std::cerr << "Falta el directorio de trabajo, el id, el archivo con la lista de grupos" << std::endl;
		return -1;
	}
	char id;

	id = atoi(argv [2]);

	chdir("local_broker");
	/*if (chdir("local_broker") != 0) {
	 throw GenericError("Cannot change working dir to %s", "local_broker");
	 }*/

	create_if_not_exists(std::string(argv [1]).append(PATH_COLA_TOKEN_MANAGER).c_str());

	TokenManager manager(argv [1], /*id,*/argv [3]);

	manager.run();

	/*socket.receivesome(debug, 200);
	 std::cout << "Recibido: " << debug << std::endl;
	 snprintf(debug, 200, "%s", "RECIBIDO!");
	 socket.sendsome(debug, strlen(debug));*/

}
catch (OSError & error) {
	std::cerr << "Error Token Manager" << std::endl;
	std::cerr << error.what() << std::endl;
	Log::crit("%s", error.what());
}
catch (const std::exception & e) {
	std::cerr << "Error Token Manager" << std::endl;
	std::cerr << e.what() << std::endl;
	Log::crit("%s", e.what());
}
catch (...) {
	std::cerr << "Error Broker Local" << std::endl;
	Log::crit("Critical error. Unknow exception at the end of the 'main' function.");
}
