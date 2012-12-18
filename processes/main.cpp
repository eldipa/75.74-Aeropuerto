#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <sstream>

#include "log.h"

#include "process.h"
#include "constants.h"
#include "messagequeue.h"
#include "daemon.h"
#include "conexiones_aeropuerto.h"

#include "database.h"
#include "stmt.h"
#include "tupleiter.h"

#include <list>

static char buffer_num_vuelo[20];

static char directorio_de_trabajo[MAX_PATH_SIZE];
static char id_productor_cinta_central[10];

static char *args_controlador_vuelo[] = { (char*) "controlador_de_vuelo", directorio_de_trabajo,
		(char*) buffer_num_vuelo, NULL };

static char *args_puesto_checkin1[] = { (char*) "puesto_checkin_1", directorio_de_trabajo,
		(char*) "1", NULL };

static char *args_puesto_checkin2[] = { (char*) "puesto_checkin_2", directorio_de_trabajo,
		(char*) "2", NULL };

static char *args_puesto_checkin3[] = { (char*) "puesto_checkin_3", directorio_de_trabajo,
		(char*) "3", NULL };

static char *args_robot_checkin[] = { (char*) "robot_checkin_1", directorio_de_trabajo, (char*) "1",
		(char*) "1", (char*) "1", NULL };

static char *args_scanner1[] = { (char*) "robot_scanner_1", directorio_de_trabajo, (char*) "1",
		(char*) "1", (char*) "1", NULL };

static char *args_scanner2[] = { (char*) "robot_scanner_1", directorio_de_trabajo, (char*) "2",
		(char*) "1", (char*) "1", NULL };

static char *args_robot_despacho1[] = { (char*) "robot_despacho_1", directorio_de_trabajo,
		(char*) "1", (char*) "1", (char*) "2", NULL };

static char *args_robot_despacho2[] = { (char*) "robot_despacho_2", directorio_de_trabajo,
		(char*) "2", (char*) "3", (char*) "4", NULL };

static char *args_robot_carga1[] = { (char*) "robot_carga_1", directorio_de_trabajo, (char*) "1",
		NULL };

static char *args_robot_carga2[] = { (char*) "robot_carga_2", directorio_de_trabajo, (char*) "2",
		NULL };

static char *args_robot_carga3[] = { (char*) "robot_carga_3", directorio_de_trabajo, (char*) "3",
		NULL };

static char *args_robot_carga4[] = { (char*) "robot_carga_4", directorio_de_trabajo, (char*) "4",
		NULL };

static char *args_robot_sospechosos[] = { (char*) "robot_control_equipaje_sospechoso_5",
		directorio_de_trabajo, (char*) "3", id_productor_cinta_central, NULL };

static char * args_generador_vuelos_trasbordo[] = { (char*) "generador_vuelos_trasbordo",
		directorio_de_trabajo, (char*) "./entrada/vuelos_entrantes.csv",
		(char*) "./entrada/vuelos_interconexion.csv", NULL };

static char *args_escucha_zonas_asignadas[] = { (char*) "escucha_zonas_asignadas",
		directorio_de_trabajo, NULL };

static char *args_torre_de_control[] = { (char*) "torre_de_control_1", directorio_de_trabajo, NULL };

static char *args_tractor1[] = { (char*) "tractor_1", directorio_de_trabajo, (char*) "1", NULL };

static char *args_tractor2[] = { (char*) "tractor_2", directorio_de_trabajo, (char*) "2", NULL };

static char *args_avion1[] = { (char*) "avion_1", directorio_de_trabajo, (char*) "1", NULL };

static char *args_avion2[] = { (char*) "avion_2", directorio_de_trabajo, (char*) "2", NULL };

/*
 * Lanza todos los vuelos registrados.
 * A medida que se liberen las zonas, los vuelos van saliendo.
 **/
void lanzar_vuelos();
void lanzar_vuelo(int num_vuelo);

int main(int argc, char** argv)
try {

	//be_a_daemon();

	ignore_signals();
	Log::notice("Iniciando...");
	Log::info("Creando aeropuerto...");
	std::list<Process> processes;

	{

		// if (argc > 1) {
		// 	strncpy(directorio_de_trabajo, argv[1], MAX_PATH_SIZE);
		// } else {
      //    strncpy(directorio_de_trabajo, PATH_KEYS, MAX_PATH_SIZE);
		// 	//strncpy(directorio_de_trabajo,"./locks",MAX_PATH_SIZE);

		// 	// Por ahora hasta poder cambiar el directorio de trabajo

		// }

      strncpy(directorio_de_trabajo, PATH_KEYS, MAX_PATH_SIZE);
		sprintf(id_productor_cinta_central, "%d", MAX_SCANNERS + 1);

		ConexionesAeropuerto aeropuerto(directorio_de_trabajo);

		Log::info("iniciando simulación...");

		processes.push_back(Process("puesto_checkin", args_puesto_checkin1));
		processes.push_back(Process("puesto_checkin", args_puesto_checkin2));
		processes.push_back(Process("puesto_checkin", args_puesto_checkin3));
		processes.push_back(Process("robot_checkin", args_robot_checkin));
		processes.push_back(Process("robot_scanner", args_scanner1));
		processes.push_back(Process("robot_scanner", args_scanner2));
		processes.push_back(Process("robot_despacho", args_robot_despacho1));
		processes.push_back(Process("robot_despacho", args_robot_despacho2));
		processes.push_back(Process("robot_carga", args_robot_carga1));
		processes.push_back(Process("robot_carga", args_robot_carga2));
		processes.push_back(Process("robot_carga", args_robot_carga3));
		processes.push_back(Process("robot_carga", args_robot_carga4));
		processes.push_back(Process("robot_control_equipaje_sospechoso", args_robot_sospechosos));
		processes.push_back(Process("generador_vuelos_trasbordo", args_generador_vuelos_trasbordo));
		processes.push_back(Process("escucha_zonas_asignadas", args_escucha_zonas_asignadas));
		processes.push_back(Process("torre_de_control", args_torre_de_control));
		processes.push_back(Process("tractor", args_tractor1));
		processes.push_back(Process("tractor", args_tractor2));
		processes.push_back(Process("avion", args_avion1));
		processes.push_back(Process("avion", args_avion2));

		// sin argumentos lanzo todos los vuelos posibles.
		if (argc == 1) {
			Log::info("Lanzo todos los vuelos registrados %s", argv[0]);
			lanzar_vuelos();
		} else {
         for(int i = 1; i<argc-1; i++) {
            lanzar_vuelo(i);
         }
      }

		Log::notice("Done, waiting for a SIGINT or a SIGTERM signal.");
		wait_signal();
		Log::notice("Signal recieved. Shutdown...");
	}

	sleep(5);
	Log::notice("Finalizando procesos. Enviando SIGTERM...");
	for (std::list<Process>::iterator it = processes.begin(); it != processes.end(); ++it)
		it->send_signal(SIGTERM);

	sleep(2);
	Log::notice("Finalizando procesos. Enviando SIGKILL...");
	for (std::list<Process>::iterator it = processes.begin(); it != processes.end(); ++it)
		it->send_signal(SIGKILL);

	Log::info("finalizando simulación...");

	return 0;
}

catch (const std::exception &e) {
	Log::crit("%s", e.what());
}
catch (...) {
	Log::crit("Critical error. Unknow exception at the end of the 'main' function.");
}

void lanzar_vuelos() {
	Database db("aeropuerto", true);
	yasper::ptr<Statement> query = db.statement("select id from Vuelo");

	yasper::ptr<TupleIterator> p_it = query->begin();
	yasper::ptr<TupleIterator> p_end = query->end();

	for (; (*p_it) != (*p_end); ++(*p_it)) {
		lanzar_vuelo(p_it->at<int>(0));
	}
}

void lanzar_vuelo(int num_vuelo) {
	Log::info("Lanzando vuelo %d", num_vuelo);
	std::stringstream sstr;
	sstr << num_vuelo;
	strcpy(buffer_num_vuelo, sstr.str().c_str());

	Process controlador_vuelo1("controlador_de_vuelo", args_controlador_vuelo);
}
