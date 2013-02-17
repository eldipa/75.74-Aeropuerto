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
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <unistd.h>
#include "lanzar_mensajeria.h"
#include "interfaces.h"
#include "genericerror.h"
#include "hostname.h"

static char numero_de_grupo [100];
static char path_key [FILENAME_MAX];
static char file_key [FILENAME_MAX];
static char ip_local [INET_ADDRSTRLEN];
static char ip_broadcast [INET_ADDRSTRLEN];

static char id [4];
static char directorio_de_trabajo_relativo [FILENAME_MAX];
static char nombre_del_grupo [MAX_NOMBRE_RECURSO];
static char nombre_broker_local [MAX_NOMBRE_RECURSO];

/*static char * group_sender_args [] = {
	(char*)"group_sender", directorio_de_trabajo_relativo, id, nombre_del_grupo, nombre_broker_local};

static char * group_receiver_args [] = {
	(char*)"group_receiver", directorio_de_trabajo_relativo, id, nombre_del_grupo, nombre_broker_local};
*/
void GroupCommManager::locate_dir(char launch_dir [FILENAME_MAX], char current_working_dir [FILENAME_MAX], char dir [FILENAME_MAX]) {
	struct stat buf;
	strcpy(launch_dir, current_working_dir);
	strcat(launch_dir, "/");
	strcat(launch_dir, dir);
	if (stat(launch_dir, &buf) != 0) { // no existe
		strcpy(launch_dir, current_working_dir);
		strcat(launch_dir, "/../");
		strcat(launch_dir, dir);
		if (stat(launch_dir, &buf) != 0) {
			throw GenericError("Cannot locate %s", dir);
		}
	}
}

void GroupCommManager::relativize_dir(const char directorio_de_trabajo [FILENAME_MAX], char launch_dir [FILENAME_MAX]) {
	char * dir;
	const char *dir_cmp;
	const char *dir_move;

	dir = strrchr(launch_dir, '/');
	dir++;

	if (directorio_de_trabajo [0] == '.' && directorio_de_trabajo [1] == '/') {
		dir_cmp = directorio_de_trabajo + 2;
		if (strncmp(dir_cmp, dir, strlen(dir)) == 0) {
			strcpy(directorio_de_trabajo_relativo,"./");

			dir_move = strchr(dir_cmp, '/') + 1;
			strcat(directorio_de_trabajo_relativo,dir_move);
		}else {
			strcpy(directorio_de_trabajo_relativo,directorio_de_trabajo);
		}
	}

}

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
}

void GroupCommManager::levantar_grupo(const std::string & nombre_grupo, char numero_grupo) {

	sprintf(numero_de_grupo, "%d", numero_grupo);
	sprintf(id, "%d", numero_grupo - 1);
	strncpy(nombre_del_grupo, nombre_grupo.c_str(), MAX_NOMBRE_RECURSO);

	get_local_ip(ip_local, (char *)"wlan0");
	get_bcast_ip(ip_broadcast, (char *)"wlan0");
	get_hostname(nombre_broker_local, ip_local);

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

	strcpy(file_key, current_working_dir);
	strcat(file_key, "/");
	strcat(file_key, path_key);
	lanzar_mensajeria(file_key, numero_grupo - 1, numero_grupo, ip_local, ip_broadcast);

	if (chdir(current_working_dir) != 0) {
		throw GenericError("Cannot change working dir to %s", current_working_dir);
	}

	locate_dir(launch_dir, current_working_dir, (char *)"local_broker");

	relativize_dir(directorio_de_trabajo.c_str(),launch_dir);

	if (chdir(launch_dir) != 0) {
		throw GenericError("Cannot change working dir to %s", launch_dir);
	}

	//Process("group_receiver", group_receiver_args);
	//Process("group_sender", group_sender_args);

	if (chdir(current_working_dir) != 0) {
		throw GenericError("Cannot change working dir to %s", current_working_dir);
	}

}
