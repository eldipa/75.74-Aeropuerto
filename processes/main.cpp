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


char *args_puesto_checkin1[] = { (char*) "puesto_checkin", 
                                (char*) "1", NULL };

char *args_puesto_checkin2[] = { (char*) "puesto_checkin", 
                                 (char*) "2", NULL };

char *args_puesto_checkin3[] = { (char*) "puesto_checkin", 
                                (char*) "3", NULL };

char *args_robot_checkin[] = { (char*) "robot_checkin", (char*) "1", // ID
		(char*) "1", (char*) "1", NULL };

char *args_scanner[] = { (char*) "robot_scanner", (char*) "1", // ID
		(char*) "1", (char*) "1", NULL };

char *args_robot_despacho[] = { (char*) "robot_despacho", (char*) "1", // ID
		(char*) "1", (char*) "2", NULL };

char *args_robot_carga0[] = { (char*) "robot_carga", (char*) "1", NULL };

char *args_robot_carga1[] = { (char*) "robot_carga", (char*) "2", NULL };

char *args_robot_sospechosos[] = { (char*) "robot_control_equipaje_sospechoso", (char*) "2",
		(char*) "5", // ID
		NULL };

char *args_robot_intercargo[] = { (char*) "robot_intercargo", (char*) "1", // ID
		NULL };

char *args_torre_de_control[] = { (char*) "torre_de_control", NULL };

char *args_tractor1[] = { (char*) "tractor", (char*) "1", NULL };

char *args_tractor2[] = { (char*) "tractor", (char*) "2", NULL };

char *args_avion1[] = { (char*) "avion", (char*) "1", NULL };

char *args_avion2[] = { (char*) "avion", (char*) "2", NULL };

/*
 * Lanza todos los vuelos registrados.
 * A medida que se liberen las zonas, los vuelos van saliendo.
 **/
void lanzar_vuelos();
void lanzar_vuelo(int num_vuelo);

//Dummy signal handler
void dummy(int) {
}

void liberar_zonas();

int main(int argc, char** argv) try {

	//be_a_daemon();

	//Ignoring the Interrupt Signal
	//In fact, the signal is catched but its handler is useless.

	struct sigaction ignore;
	memset(&ignore, 0, sizeof(struct sigaction));
	ignore.sa_handler = &dummy;

	sigaction(SIGINT, &ignore, 0);

	Log::info("Creando aeropuerto...");

	ConexionesAeropuerto aeropuerto(PATH_KEYS);

	Log::info("iniciando simulación...");

   liberar_zonas();

	Process puesto_checkin1("puesto_checkin", args_puesto_checkin1);
	Process puesto_checkin2("puesto_checkin", args_puesto_checkin2);
	Process puesto_checkin3("puesto_checkin", args_puesto_checkin3);

	Process robot_checkin("robot_checkin", args_robot_checkin);
	Process robot_scanner("robot_scanner", args_scanner);
	Process robot_despacho("robot_despacho", args_robot_despacho);
	Process robot_carga0("robot_carga", args_robot_carga0);
	Process robot_carga1("robot_carga", args_robot_carga1);
	Process robot_sospechosos("robot_control_equipaje_sospechoso", args_robot_sospechosos);
	Process robot_intercargo("robot_intercargo", args_robot_intercargo);
	Process torre_de_control("torre_de_control", args_torre_de_control);
	Process tractor1("tractor", args_tractor1);
	Process tractor2("tractor", args_tractor2);
	Process avion1("avion", args_avion1);
	Process avion2("avion", args_avion2);

   // sin argumentos lanzo todos los vuelos posibles.
   if(argc == 1) {
      Log::info("Lanzo todos los vuelos registrados %s", argv[0]);
      lanzar_vuelos();
   } 

	Log::info("Done, waiting for a SIGINT signal.");
	pause();
	Log::info("Signal recieved. Shutdown...");

	Log::info("finalizando robots...");

	puesto_checkin1.send_signal(SIGTERM);
	puesto_checkin2.send_signal(SIGTERM);
	puesto_checkin3.send_signal(SIGTERM);
	robot_checkin.send_signal(SIGTERM);
	robot_scanner.send_signal(SIGTERM);
	robot_despacho.send_signal(SIGTERM);
	robot_carga0.send_signal(SIGTERM);
	robot_carga1.send_signal(SIGTERM);
	robot_sospechosos.send_signal(SIGTERM);
	robot_intercargo.send_signal(SIGTERM);
	torre_de_control.send_signal(SIGTERM);
	tractor1.send_signal(SIGTERM);
	tractor2.send_signal(SIGTERM);
	avion1.send_signal(SIGTERM);
	avion2.send_signal(SIGTERM);

	Log::info("finalizando simulación...");

	return 0;
} catch(const std::exception &e) {
   Log::crit("%s", e.what());
} catch(...) {
   Log::crit("Critical error. Unknow exception at the end of the 'main' function.");
}

/*
 * Borra todas las zonas habilitadas en la BD.
 * Esto hay que cambiarlo por una shared memory con el robot_despacho.
 **/
void liberar_zonas() {
	Database db("aeropuerto", false);

	yasper::ptr<Statement> query = db.statement("delete from ZonasUtilizadas");
	yasper::ptr<TupleIterator> p_it = query->begin();
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
   char buffer_num_vuelo[20];
   std::stringstream sstr;sstr<<num_vuelo;
   strcpy(buffer_num_vuelo, sstr.str().c_str());

   char *args_controlador_vuelo[] = { (char*) "controlador_de_vuelo", (char*)buffer_num_vuelo, NULL };

   Process controlador_vuelo1("controlador_de_vuelo", args_controlador_vuelo);
}
