/*
 * SemaphoreSetDistribuido.cpp
 *
 *  Created on: 18/02/2013
 *      Author: gonzalo
 */

#include "semaphore_set_distribuido.h"
#include <cstdio>
#include "process.h"
#include "daemon.h"
#include "dir.h"
#include <csignal>
#include <sstream>

static char directorio [FILENAME_MAX];
static char nombre_app [MAX_NOMBRE_RECURSO];
static char brokers_file [FILENAME_MAX];
static char grupo [MAX_NOMBRE_RECURSO];
static char id_ipc [4];
static char tamanio_mem [10];
static char num_sem [6];
static char cant_sems [6];

static char * args_local_broker_comm [] = {
	(char *)"local_broker_comm", directorio, nombre_app, brokers_file, grupo, id_ipc, tamanio_mem, (char*)"sem",
	num_sem, cant_sems, NULL};

#define MAX_SEMS 125

SemaphoreSetDistribuido::SemaphoreSetDistribuido(std::vector<unsigned short> & numeros_de_semaforo,
	const std::string & directorio_de_trabajo, const std::string & nombre_app, const std::string & nombre_grupo,
	char id, int cantidad_sems)
	: nombre_recurso(nombre_grupo), cantidad_de_semaforos(cantidad_sems)
{
	std::string prefijo_archivo;
	prefijo_archivo = std::string(directorio_de_trabajo).append(PREFIJO_RECURSO).append(nombre_grupo);
	char numero [5];
	unsigned short valor;

	if (cantidad_de_semaforos > 125) {
		throw GenericError("Cannot allocate %d semaphores, max 125", cantidad_de_semaforos);
	}

	std::vector<unsigned short> valores(2 * cantidad_de_semaforos, 0);

	create_if_not_exists(std::string(prefijo_archivo).append(POSTFIJO_LCK).c_str());

	semaforos = new SemaphoreSet(valores, std::string(prefijo_archivo).append(POSTFIJO_LCK).c_str(), id, 0664);

	control = ControlTokens::get_instance(directorio_de_trabajo, true);

	for (size_t i = 0 ; i < numeros_de_semaforo.size() ; i++) {

		valor = numeros_de_semaforo [i];
		sprintf(numero, "%d", valor);

		lanzar_comunicacion(directorio_de_trabajo, nombre_app,
			std::string(directorio_de_trabajo).append(PATH_LOCAL_BROKER_LIST_FILE).c_str(),
			std::string(nombre_grupo).append(numero), 0, 0, valor);
	}
}

SemaphoreSetDistribuido::SemaphoreSetDistribuido(const std::string & directorio_de_trabajo,
	const std::string & nombre_grupo, char id, int cantidad_semaforos)
	: nombre_recurso(nombre_grupo), cantidad_de_semaforos(cantidad_semaforos)
{
	std::string prefijo_archivo;
	prefijo_archivo = std::string(directorio_de_trabajo).append(PREFIJO_RECURSO).append(nombre_grupo);

	create_if_not_exists(std::string(prefijo_archivo).append(POSTFIJO_LCK).c_str());

	control = ControlTokens::get_instance(directorio_de_trabajo);
	semaforos = new SemaphoreSet(std::string(prefijo_archivo).append(POSTFIJO_LCK).c_str(), id, 0);

}

SemaphoreSetDistribuido::~SemaphoreSetDistribuido() {
	std::vector<Process *>::iterator i;
	try {
		for (i = handlers.begin(); i != handlers.end() ; i++) {
			(*i)->send_signal(SIGTERM, false);
			(*i)->wait();
		}
	} catch (OSError & error) {
	}
	ControlTokens::destroy_instance();
	delete semaforos;
}

void SemaphoreSetDistribuido::wait_on(int num_sem) {
	std::stringstream ss;
	ss << num_sem;
	control->cargar_esperando_token(std::string(nombre_recurso).append(ss.str()).c_str());
	semaforos->wait_on(num_sem);
}

void SemaphoreSetDistribuido::signalize(int num_sem) {
	control->limpiar_esperando_token();
	semaforos->signalize(num_sem + cantidad_de_semaforos);
}

void SemaphoreSetDistribuido::entregar_token(int num_sem) {
	semaforos->signalize(num_sem);
}

void SemaphoreSetDistribuido::esperar_token(int num_sem) {
	semaforos->wait_on(num_sem + cantidad_de_semaforos);
}

void SemaphoreSetDistribuido::forwardear_token(int num_sem) {
	semaforos->signalize(num_sem + cantidad_de_semaforos);
}

void SemaphoreSetDistribuido::lanzar_comunicacion(const std::string & directorio_de_trabajo,
	const std::string & nombre_aplicacion, const std::string & local_brokers_file, const std::string & nombre_grupo,
	char id, size_t tamanio_memoria, int numero_semaforo)
{
	char current_working_dir [FILENAME_MAX];
	char launch_dir [FILENAME_MAX];

	ignore_signals();

	snprintf(directorio, FILENAME_MAX, "%s", directorio_de_trabajo.c_str());
	snprintf(nombre_app, MAX_NOMBRE_RECURSO, "%s", nombre_aplicacion.c_str());
	snprintf(brokers_file, FILENAME_MAX, "%s", local_brokers_file.c_str());
	snprintf(grupo, MAX_NOMBRE_RECURSO, "%s", nombre_grupo.c_str());
	snprintf(id_ipc, 4, "%d", id);
	snprintf(num_sem, 6, "%d", numero_semaforo);
	snprintf(cant_sems, 6, "%d", cantidad_de_semaforos);

#ifdef __x86_64__
	snprintf(tamanio_mem, sizeof(tamanio_mem), "%lu", tamanio_memoria);
#else
	snprintf(tamanio_mem, sizeof(tamanio_mem), "%u", tamanio_memoria);
#endif

	if (!getcwd(current_working_dir, sizeof(current_working_dir)))
		throw GenericError("Unable to get current working dir");
	current_working_dir [sizeof(current_working_dir) - 1] = '\0';

	locate_dir(launch_dir, current_working_dir, (char *)"clientes");

	relativize_dir(directorio, directorio_de_trabajo.c_str(), launch_dir, current_working_dir);
	relativize_dir(brokers_file, local_brokers_file.c_str(), launch_dir, current_working_dir);

	ignore_childs();

	if (chdir(launch_dir) != 0) {
		throw GenericError("Cannot change working dir to %s", launch_dir);
	}

	handlers.push_back(new Process("local_broker_comm", args_local_broker_comm));

	if (chdir(current_working_dir) != 0) {
		throw GenericError("Cannot change working dir to %s", current_working_dir);
	}

}

