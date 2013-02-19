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
/*static char * args_local_broker_comm [] = {
 (char*)"/home/gonzalo/workspaces/git/75.74-Aeropuerto/clientes/local_broker_comm", directorio_de_trabajo, app_name,
 (char*)"localbroker1.sitio1.aeropuerto1:1234", nombre_grupo, id, tamanio, NULL};
 */
int main(int argc, char * argv [])
try
{
	char id_recurso;
	if (argc < 2) {
		std::cerr << "Faltan argumentos " << argv [0] << std::endl;
		return -1;
	}

	strncpy(app_name, argv [1], 200);

	if (strncmp(argv [1], "escaner1", MAX_NOMBRE_RECURSO) == 0) {
		//snprintf(directorio_de_trabajo, 200, "%s", "/home/gonzalo/workspaces/git/75.74-Aeropuerto/clientes/locks");
		snprintf(directorio_de_trabajo, 200, "%s", "./processes/locks");
	} else {
		snprintf(directorio_de_trabajo, 200, "%s", "/home/gonzalo/workspaces/git/75.74-Aeropuerto/clientes/locks2");
	}

	// mismo tamanño que en group_lists.txt en local broker
	// faltaria inicializar la memoria
	id_recurso = 0;
	sprintf(id, "%d", id_recurso);
	sprintf(nombre_grupo, "%s", "cinta_principal");
	sprintf(tamanio, "%d", 0);
	sprintf(tamanio, "%d", 256);
	//MemoriaDistribuida memoria(directorio_de_trabajo, nombre_grupo, id_recurso, 256);
	//char * pointer;
	//int * a;

	sprintf(nombre_grupo, "%s", "cinta_principal");
	MutexDistribuido mutex(directorio_de_trabajo, nombre_grupo, id_recurso, true);
	mutex.lanzar_comunicacion(directorio_de_trabajo, "escaner1", "localbroker1.sitio1.aeropuerto1:1234",
		"cinta_principal", 0, 0);

	//Process p("/home/gonzalo/workspaces/git/75.74-Aeropuerto/clientes/local_broker_comm", args_local_broker_comm);

	//pointer = (char*)memoria.memory_pointer();
	//a = reinterpret_cast<int *>(memoria.memory_pointer());

	//a = (int *)pointer;

	//while (*a < 20) {

	//memoria.lock();

	//(*a)++;
	//std::cout << argv [1] << ":" << *a << std::endl;

	/*for (int i = 0 ; i < 30 ; i++) {

		std::cout << a [i] << std::endl;

	}*/

	//memoria.unlock();
	//}

	/*for (int i = 0 ; i < 2 ; i++) {
	 mutex.lock();

	 std::cout << "A" << std::endl;

	 mutex.unlock();
	 }*/

}
catch (const std::exception &e) {
	Log::crit("%s", e.what());
}
catch (...) {
	Log::crit("Critical error. Unknow exception at the end of the 'main' function.");
}
