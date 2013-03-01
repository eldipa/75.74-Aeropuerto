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
#include "semaphore_set_distribuido.h"
#include "mensajes.h"

static char directorio_de_trabajo [200];
static char app_name [200];
static char nombre_grupo [200];
static char id [10];
static char tamanio [10];
/*static char * args_local_broker_comm [] = {
 (char*)"/home/gonzalo/workspaces/git/75.74-Aeropuerto/clientes/local_broker_comm", directorio_de_trabajo, app_name,
 (char*)"localbroker1.sitio1.aeropuerto1:1234", nombre_grupo, id, tamanio, NULL};
 */

int test_semaphore_set(int argc, char *argv []) {
	std::vector<unsigned short> valores(2, 1);
	//char id_recurso;

	if (argc < 2) {
		std::cerr << "Faltan argumentos " << argv [0] << std::endl;
		return -1;
	}

	//std::cout << valores.size() << std::endl;

	snprintf(directorio_de_trabajo, 200, "%s", "./locks");
	strncpy(app_name, argv [1], 200);
	sprintf(nombre_grupo, "%s", "sem_cinta_principal_");

	SemaphoreSetDistribuido semaforos(valores, directorio_de_trabajo, app_name, nombre_grupo, true);
	std::cout << "semaforos creados" << std::endl;
	semaforos.wait_on(0);
	std::cout << "Token1" << std::endl;
	semaforos.wait_on(1);
	std::cout << "Token2" << std::endl;
	semaforos.signalize(1);
	std::cout << "Release Token2" << std::endl;
	semaforos.signalize(0);
	std::cout << "Release Token1" << std::endl;

	return 0;
}

void test_zona_asignada(int argc, char * argv []) {
	if (argc < 3) {
		std::cerr << "Faltan argumentos " << argv [0] << std::endl;
		return;
	}
	MENSAJE_ZONA_ASIGNADA mensaje;

	MessageQueue cola_asignaciones(std::string(argv [1]).append(PATH_COLA_ESCUCHA_ZONA_ASIGNADA).c_str(), 0);
	mensaje.zona_asignada = 1;
	mensaje.numero_de_vuelo = 1;
	mensaje.mtype = 1;
	cola_asignaciones.push(&mensaje, sizeof(MENSAJE_ZONA_ASIGNADA) - sizeof(long));
}
/*
void test_local_broker_comm(int argc, char * argv []) {

	argc = argc + 1 - 1;
	argv = argv + 1 - 1;
	LocalBrokerComm broker("debug", "localbroker1.sitio1.aeropuerto1", "1234");

	broker.join("cinta_principal", mensajes::JOIN);
}
*/
int main(int argc, char * argv [])
try
{
//	char current_working_dir [FILENAME_MAX];
	char id_recurso;
	if (argc < 2) {
		std::cerr << "Faltan argumentos " << argv [0] << std::endl;
		return -1;
	}
	id_recurso = 0;
	sprintf(id, "%d", id_recurso);
	sprintf(nombre_grupo, "%s", "cinta_principal");
	sprintf(tamanio, "%d", 0);
	sprintf(tamanio, "%d", 256);

	/*if (!getcwd(current_working_dir, sizeof(current_working_dir)))
	 throw GenericError("Unable to get current working dir");
	 current_working_dir [sizeof(current_working_dir) - 1] = '\0';
	 strcat(current_working_dir, "/processes");
	 chdir(current_working_dir);*/

	if (chdir("processes") != 0) {
		throw GenericError("Cannot change working dir to %s", "local_broker");
	}

	//test_semaphore_set(argc, argv);
	//test_zona_asignada(argc, argv);
	//test_local_broker_comm(argc, argv);

	/*if (strncmp(argv [1], "escaner1", MAX_NOMBRE_RECURSO) == 0) {
	 //snprintf(directorio_de_trabajo, 200, "%s", "/home/gonzalo/workspaces/git/75.74-Aeropuerto/clientes/locks");
	 snprintf(directorio_de_trabajo, 200, "%s", "./processes/locks");
	 } else {
	 snprintf(directorio_de_trabajo, 200, "%s", "/home/gonzalo/workspaces/git/75.74-Aeropuerto/clientes/locks2");
	 }*/

	// mismo tamaño que en group_lists.txt en local broker
	// faltaria inicializar la memoria
	//MemoriaDistribuida memoria(directorio_de_trabajo, nombre_grupo, id_recurso, 256);
	//char * pointer;
	//int * a;
	//MutexDistribuido mutex(directorio_de_trabajo, nombre_grupo, id_recurso, true);
	//mutex.lanzar_comunicacion(directorio_de_trabajo, "escaner1", "localbroker1.sitio1.aeropuerto1:1234",
	//	"cinta_principal", 0, 0);
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
	std::cerr << "Unable to connect to local_brokers" << std::endl;
	Log::crit("%s", e.what());
}
catch (...) {
	Log::crit("Critical error. Unknow exception at the end of the 'main' function.");
}
