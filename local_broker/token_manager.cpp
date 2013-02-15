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
#include <fcntl.h>
#include "log.h"
#include "oserror.h"
#include "genericerror.h"
#include <unistd.h>
#include <sys/stat.h>
#include <cstring>
#include "daemon.h"

TokenManager::TokenManager(const std::string & directorio_de_trabajo, char id, const std::string & groups_file)
	: clientes(std::string(directorio_de_trabajo).append(PATH_COLA_TOKEN_MANAGER).c_str(), id, 0664, true),
		outbound(std::string(directorio_de_trabajo).append(PATH_COLAS_BROKERS).c_str(), id ),
		inbound(std::string(directorio_de_trabajo).append(PATH_COLAS_BROKERS).c_str(), id + 128)
{
	id = id + 1 - 1;
	crear_grupos(directorio_de_trabajo, groups_file);
}

TokenManager::~TokenManager() {
	Grupo * g;
	std::map<std::string, Grupo *>::iterator i;

	for (i = this->grupos.begin(); i != this->grupos.end() ; i++) {
		g = i->second;
		delete g;
	}
}

void TokenManager::crear_grupos(const std::string & directorio_de_trabajo, const std::string & groups_file) {
	FILE * f;
	char nombre_recurso [MAX_NOMBRE_RECURSO];
	char primera_linea [200];
	int tamanio_memoria;
	int file;

	f = fopen(groups_file.c_str(), "rt");

	if (f == NULL) {
		throw OSError("No se pudieron crear los grupos: %s", groups_file.c_str());
	}
	// evito la primera linea
	fscanf(f, "%s\n", primera_linea);

	while (fscanf(f, "%[^:]:%d\n", nombre_recurso, &tamanio_memoria) != EOF) {
		// Crear Grupo
		Grupo * g;
		// Creo el archivo lck
		struct stat buf;
		char path [200];
		strcpy(path, directorio_de_trabajo.c_str());
		strcat(path, PREFIJO_RECURSO);
		strcat(path, nombre_recurso);
		strcat(path, ".lck");
		if (stat(path, &buf) != 0) {
			file = open(path, O_CREAT | 0664);
			if (file != -1) {
				close(file);
			} else {
				//THROW OSERROR
			}
		}
		g = new Grupo(directorio_de_trabajo, nombre_recurso, tamanio_memoria, true);
		grupos.insert(std::pair<std::string, Grupo *>(std::string(nombre_recurso), g));
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

	//ignore_signals();

	char data [30];

	inbound.pull(data,30,0);

	std::cout << data << std::endl;
	do {
		try {
			// Recibo todos los tokens
			clientes.pull(&mensaje, sizeof(traspaso_token_t) - sizeof(long), 0);
			//Me fijo que token me dieron
			recurso.assign(mensaje.recurso);
			if (grupos.count(recurso) < 1) {
				throw GenericError("Error Grupo %s no encontrado", mensaje.recurso);
			}
			g = grupos.at(recurso);
			sleep(1);
			g->pasar_token_a_proximo_cliente();
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

	TokenManager manager(argv [1], id, argv [3]);

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
