/*
 * MutexDistribuido.cpp
 *
 *  Created on: 12/02/2013
 *      Author: gonzalo
 */

#include "mutex_distribuido.h"
#include "constants.h"

MutexDistribuido::MutexDistribuido(const std::string & directorio_de_trabajo, const std::string & nombre_grupo, char id,
	bool create)
	: nombre_recurso(nombre_grupo)

{

	// Creo el archivo lck
	int file;
	struct stat buf;
	char path [200];
	strcpy(path, directorio_de_trabajo.c_str());
	strcat(path, PREFIJO_RECURSO);
	strcat(path, nombre_grupo.c_str());
	strcat(path, POSTFIJO_LCK);
	if (stat(path, &buf) != 0) {
		file = open(path, O_CREAT | 0664);
		if (file != -1) {
			close(file);
		} else {
			//THROW OSERROR
		}
	}

	if (create) {
		mutex =
			new SemaphoreSet(std::vector<short unsigned int>(2, 0),
				std::string(directorio_de_trabajo).append(PREFIJO_RECURSO).append(nombre_grupo).append(POSTFIJO_LCK).c_str(),
				id, 0664);
	}

}

MutexDistribuido::MutexDistribuido(const std::string & directorio_de_trabajo, const std::string & nombre_grupo, char id)
	: nombre_recurso(nombre_grupo)

{
	mutex = new SemaphoreSet(
		std::string(directorio_de_trabajo).append(PREFIJO_RECURSO).append(nombre_grupo).append(POSTFIJO_LCK).c_str(),
		id, 0);
}

MutexDistribuido::~MutexDistribuido() {
	if (mutex)
		delete mutex;
}

void MutexDistribuido::lock(ControlTokens & control) {
	control.cargar_esperando_token(nombre_recurso.c_str());
	mutex->wait_on(0);
}

void MutexDistribuido::unlock(ControlTokens &control) {
	control.limpiar_esperando_token();
	mutex->signalize(1);
}

void MutexDistribuido::entregar_token() {
	mutex->signalize(0);
}

void MutexDistribuido::esperar_token() {
	mutex->wait_on(1);
}

