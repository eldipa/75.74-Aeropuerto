/*
 * MemoriaDistribuida.cpp
 *
 *  Created on: 12/02/2013
 *      Author: gonzalo
 */

#include "memoria_distribuida.h"
#include "constants.h"

MemoriaDistribuida::MemoriaDistribuida(const std::string & directorio_de_trabajo, const std::string & nombre_grupo,
	char id, size_t tamanio)
	: nombre_recurso(nombre_grupo), tamanio(tamanio)

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

	memoria = new SharedMemory(
		std::string(directorio_de_trabajo).append(PREFIJO_RECURSO).append(nombre_grupo).append(POSTFIJO_LCK).c_str(),
		char(2 * id), tamanio, 0664, true, false);
	mutex = new SemaphoreSet(std::vector<short unsigned int>(2, 0),
		std::string(directorio_de_trabajo).append(PREFIJO_RECURSO).append(nombre_grupo).append(POSTFIJO_LCK).c_str(),
		char(2 * id + 1), 0664);

}

MemoriaDistribuida::MemoriaDistribuida(const std::string & directorio_de_trabajo, const std::string & nombre_grupo,
	char id)
	: nombre_recurso(nombre_grupo), tamanio(0)

{

	memoria = new SharedMemory(
		std::string(directorio_de_trabajo).append(PREFIJO_RECURSO).append(nombre_grupo).append(POSTFIJO_LCK).c_str(),
		char(2 * id), 0, false, false);
	mutex = new SemaphoreSet(
		std::string(directorio_de_trabajo).append(PREFIJO_RECURSO).append(nombre_grupo).append(POSTFIJO_LCK).c_str(),
		char(2 * id + 1), 0);
}

MemoriaDistribuida::~MemoriaDistribuida() {
	if (memoria)
		delete memoria;
	if (mutex)
		delete mutex;
}

void MemoriaDistribuida::lock(ControlTokens & control) {
	control.cargar_esperando_token(nombre_recurso.c_str());
	mutex->wait_on(0);
}

void MemoriaDistribuida::unlock(ControlTokens &control) {
	control.limpiar_esperando_token();
	mutex->signalize(1);
}

void MemoriaDistribuida::entregar_token() {
	mutex->signalize(0);
}

void MemoriaDistribuida::esperar_token() {
	mutex->wait_on(1);
}

size_t MemoriaDistribuida::get_mem_size() {
	return this->tamanio;
}

void * MemoriaDistribuida::memory_pointer() {
	return this->memoria->memory_pointer();
}
