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

static char numero_de_grupo [100];
static char path_key [200];
static char id_key [4];
static char ip_local [16];
static char ip_broadcast [16];
static char inbound_path [200];

static char * python_args [] = {(char *)"/usr/bin/python", inbound_path, path_key, id_key, numero_de_grupo, ip_local, ip_broadcast};

GroupCommManager::GroupCommManager(const std::string & directorio_de_trabajo, const std::string & path_to_inbound) {
	
	strcpy(inbound_path, path_to_inbound.c_str());
	strcpy(path_key, directorio_de_trabajo.c_str());
	strcat(path_key, PATH_COLAS_GRUPO);
	struct stat buf;

	int file;
	if (stat(path_key, &buf) != 0) {
		file = open(path_key, O_CREAT | 0664);
		if (file != -1) {
			close (file);
		} else {
			//THROW OSERROR
		}
	}

}

GroupCommManager::~GroupCommManager() {
	// TODO Auto-generated destructor stub
}

void GroupCommManager::levantar_grupo(const std::string & nombre_grupo, int numero_grupo) {
	std::string a;
	a = nombre_grupo + "";
	sprintf(numero_de_grupo, "%d", numero_grupo);

	Process p("python", python_args);
}
