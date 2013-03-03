/*
 * GroupCommManager.cpp
 *
 *  Created on: 16/02/2013
 *      Author: gonzalo
 */

#include "group_comm_manager.h"
#include "process.h"
#include <cstring>
#include <cstdio>
#include "local_broker_constants.h"
#include "lanzar_mensajeria.h"
#include "interfaces.h"
#include "genericerror.h"
#include "valueerror.h"
#include "hostname.h"
#include "dir.h"
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <csignal>

static char numero_de_grupo [100];
static char path_key [FILENAME_MAX];
static char file_key [FILENAME_MAX];
static char ip_local [INET_ADDRSTRLEN];
static char ip_broadcast [INET_ADDRSTRLEN];

static char id [4];
static char directorio_de_trabajo_relativo [FILENAME_MAX];
static char nombre_del_grupo [MAX_NOMBRE_RECURSO];
static char nombre_broker_local [MAX_NOMBRE_RECURSO];
static char token [2];

 static char * group_sender_args [] = {
 (char*)"group_sender", directorio_de_trabajo_relativo, id, nombre_del_grupo, nombre_broker_local, NULL};

 static char * group_receiver_args [] = {
 (char*)"group_receiver", directorio_de_trabajo_relativo, id, nombre_del_grupo, nombre_broker_local, token, NULL};

 static char * multiplexer_args [] = {(char*)"multiplexer", directorio_de_trabajo_relativo, NULL};

GroupCommManager::GroupCommManager(const std::string & directorio_trabajo)
	: directorio_de_trabajo(directorio_trabajo)
{

	strcpy(directorio_de_trabajo_relativo, directorio_trabajo.c_str());
	strcpy(path_key, directorio_trabajo.c_str());
	strcat(path_key, PATH_COLAS_BROKERS);
	struct stat buf;

	int file;
	if (stat(path_key, &buf) != 0) {
		file = open(path_key, O_CREAT | 0664);
		if (file != -1) {
			close(file);
		} else {
			//THROW OSERROR
		}
	}
}

GroupCommManager::~GroupCommManager() {
	std::vector<Process *>::iterator i;

	for (i = procesos.begin(); i < procesos.end() ; i++) {
		(*i)->send_signal(SIGTERM, false);
		(*i)->wait();
		delete *i;
	}
}

void GroupCommManager::levantar_multiplexor(char id_aeropuerto) {

	if (id_aeropuerto < 1) {
		throw ValueError("Id anillo inválido (%d)", id_aeropuerto);
	}
	sprintf(numero_de_grupo, "%d", id_aeropuerto);

#ifdef __x86_64__
	get_local_ip(ip_local, (char *)"wlan0");
	get_bcast_ip(ip_broadcast, (char *)"wlan0");
#else
	get_local_ip(ip_local, (char *) "eth0");
	get_bcast_ip(ip_broadcast, (char *) "eth0");
#endif
	//get_hostname(nombre_broker_local, ip_local);
	strcpy(nombre_broker_local, "localbroker1");
	if (strchr(nombre_broker_local, '.') != NULL)
		*strchr(nombre_broker_local, '.') = '\0';

	char current_working_dir [FILENAME_MAX];
	char launch_dir [FILENAME_MAX];

	if (!getcwd(current_working_dir, sizeof(current_working_dir)))
		throw GenericError("Unable to get current working dir");
	current_working_dir [sizeof(current_working_dir) - 1] = '\0'; /* not really required */

	locate_dir(launch_dir, current_working_dir, (char *)"group");

	if (chdir(launch_dir) != 0) {
		throw GenericError("Cannot change working dir to %s", launch_dir);
	}

	if (path_key [0] != '/') {
		strcpy(file_key, current_working_dir);
		strcat(file_key, "/");
		strcat(file_key, path_key);
	} else {
		strcpy(file_key, path_key);
	}
	Log::info("Lanzo mensajeria por anillo (id_aeropuerto = %d)\n", id_aeropuerto);
	lanzar_mensajeria(file_key, 0, id_aeropuerto, ip_local, ip_broadcast);

	if (chdir(current_working_dir) != 0) {
		throw GenericError("Cannot change working dir to %s", current_working_dir);
	}

	Log::info("Lanzo multiplexor por anillo (id_aeropuerto = %d)\n", id_aeropuerto);
	procesos.push_back(new Process("multiplexer", multiplexer_args));
}

void GroupCommManager::levantar_grupo(const std::string & nombre_grupo, int crear_token) {

	sprintf(id, "%d", 0);
	strncpy(nombre_del_grupo, nombre_grupo.c_str(), MAX_NOMBRE_RECURSO);
	sprintf(token, "%d", crear_token);

	//get_hostname(nombre_broker_local, ip_local);
	strcpy(nombre_broker_local, "localbroker1");
	if (strchr(nombre_broker_local, '.') != NULL)
		*strchr(nombre_broker_local, '.') = '\0';

	char current_working_dir [FILENAME_MAX];
	char launch_dir [FILENAME_MAX];

	if (!getcwd(current_working_dir, sizeof(current_working_dir)))
		throw GenericError("Unable to get current working dir");
	current_working_dir [sizeof(current_working_dir) - 1] = '\0'; /* not really required */

	locate_dir(launch_dir, current_working_dir, (char *)"local_broker");

	relativize_dir(directorio_de_trabajo_relativo, directorio_de_trabajo.c_str(), (const char *)launch_dir,
		current_working_dir);

	if (chdir(launch_dir) != 0) {
		throw GenericError("Cannot change working dir to %s", launch_dir);
	}

	Log::info("Lanzo procesos administradores de anillo %s\n", nombre_grupo.c_str());

	procesos.push_back(new Process("group_receiver", group_receiver_args));
	procesos.push_back(new Process("group_sender", group_sender_args));

	if (chdir(current_working_dir) != 0) {
		throw GenericError("Cannot change working dir to %s", current_working_dir);
	}

}

void GroupCommManager::levantar_servicio() {
#ifdef __x86_64__
	get_local_ip(ip_local, (char *)"wlan0");
	get_bcast_ip(ip_broadcast, (char *)"wlan0");
#else
	get_local_ip(ip_local, (char *) "eth0");
	get_bcast_ip(ip_broadcast, (char *) "eth0");
#endif
	//get_hostname(nombre_broker_local, ip_local);
	strcpy(nombre_broker_local, "localbroker1");
	char current_working_dir [FILENAME_MAX];
	char launch_dir [FILENAME_MAX];

	if (!getcwd(current_working_dir, sizeof(current_working_dir))) {
		throw GenericError("Unable to get current working dir");
	}
	locate_dir(launch_dir, current_working_dir, (char *)"group");

	if (chdir(launch_dir) != 0) {
		throw GenericError("Cannot change working dir to %s", launch_dir);
	}
	Log::info("Lanzo beacon service\n");
	lanzar_beacon_svc(ip_local);

	if (chdir(current_working_dir) != 0) {
		throw GenericError("Cannot change working dir to %s", current_working_dir);
	}

}

void GroupCommManager::bajar_servicio() {
	bajar_grupos();
}
