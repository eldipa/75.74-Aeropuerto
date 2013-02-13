#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <vector>
#include "api_tractor.h"
#include "log.h"
#include "local_broker_comm.h"
#include "process.h"
#include "control_tokens.h"
#include "memoria_distribuida.h"
#include "mutex_distribuido.h"

static char directorio_de_trabajo [200];
static char app_name [200];
static char nombre_grupo [200];
static char id [10];
static char tamanio [10];
static char * args_local_broker_comm [] = {
	(char*)"/home/gonzalo/workspaces/git/75.74-Aeropuerto/clientes/local_broker_comm", directorio_de_trabajo, app_name,
	(char*)"localbroker1.sitio1.aeropuerto1:1234", nombre_grupo, id, tamanio, NULL};

int main(int argc, char * argv [])
try
{
	char id_recurso;
	if (argc < 2) {
		std::cerr << "Faltan argumentos " << argv [0] << std::endl;
		return -1;
	}

	//char * pointer;
	//int * a;
	strncpy(app_name, argv [1], 200);

	if (strncmp(argv [1], "escaner1", MAX_NAME_SIZE) == 0) {
		snprintf(directorio_de_trabajo, 200, "%s", "/home/gonzalo/workspaces/git/75.74-Aeropuerto/clientes/locks");
	} else {
		snprintf(directorio_de_trabajo, 200, "%s", "/home/gonzalo/workspaces/git/75.74-Aeropuerto/clientes/locks2");
	}

	ControlTokens control(directorio_de_trabajo, true);
	// mismo tamanño que en group_lists.txt en local broker
	// faltaria inicializar la memoria
	id_recurso = 1;
	sprintf(id, "%d", id_recurso);
	sprintf(nombre_grupo, "%s", "cinta_principal");
	sprintf(tamanio, "%d", 0);
	//MemoriaDistribuida memoria(directorio_de_trabajo, nombre_grupo, 0, 256);
	sprintf(nombre_grupo, "%s", "cinta_principal");
	MutexDistribuido mutex(directorio_de_trabajo, nombre_grupo, id_recurso, true);

	Process p("/home/gonzalo/workspaces/git/75.74-Aeropuerto/clientes/local_broker_comm", args_local_broker_comm);

	/*pointer = (char*)memoria.memory_pointer();
	 a = reinterpret_cast<int *>(memoria.memory_pointer());

	 *a = 0;

	 while (*a < 20) {

	 memoria.lock(control);

	 (*a)++;
	 std::cout << argv [1] << ":" << *a << std::endl;

	 memoria.unlock(control);
	 }*/

	for (int i = 0 ; i < 2 ; i++) {
		mutex.lock(control);

		std::cout << "A" << std::endl;

		mutex.unlock(control);
	}

}
catch (const std::exception &e) {
	Log::crit("%s", e.what());
}
catch (...) {
	Log::crit("Critical error. Unknow exception at the end of the 'main' function.");
}
