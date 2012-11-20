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

void liberar_zonas();

int main(int argc, char** argv) try {

	//be_a_daemon();

        ignore_signals();
        Log::notice("Iniciando...");
	Log::info("Creando aeropuerto...");
        std::list<Process> processes;
         
        {
                 ConexionesAeropuerto aeropuerto(PATH_KEYS);

                 Log::info("iniciando simulación...");

                 liberar_zonas();

                 processes.push_back(Process("puesto_checkin", args_puesto_checkin1));
                 processes.push_back(Process("puesto_checkin", args_puesto_checkin2));
                 processes.push_back(Process("puesto_checkin", args_puesto_checkin3));
                 processes.push_back(Process("robot_checkin", args_robot_checkin));
                 processes.push_back(Process("robot_scanner", args_scanner));
                 processes.push_back(Process("robot_despacho", args_robot_despacho));
                 processes.push_back(Process("robot_carga", args_robot_carga0));
                 processes.push_back(Process("robot_carga", args_robot_carga1));
                 processes.push_back(Process("robot_control_equipaje_sospechoso", args_robot_sospechosos));
                 processes.push_back(Process("robot_intercargo", args_robot_intercargo));
                 processes.push_back(Process("torre_de_control", args_torre_de_control));
                 processes.push_back(Process("tractor", args_tractor1));
                 processes.push_back(Process("tractor", args_tractor2));
                 processes.push_back(Process("avion", args_avion1));
                 processes.push_back(Process("avion", args_avion2));

                 // sin argumentos lanzo todos los vuelos posibles.
                 if(argc == 1) {
                       Log::info("Lanzo todos los vuelos registrados %s", argv[0]);
                       lanzar_vuelos();
                 } 

                 Log::notice("Done, waiting for a SIGINT or a SIGTERM signal.");
                 wait_signal();
                 Log::notice("Signal recieved. Shutdown...");
        }

        sleep(15);
	Log::notice("Finalizando procesos. Enviando SIGTERM...");
        for(std::list<Process>::iterator it = processes.begin(); it != processes.end(); ++it)
           it->send_signal(SIGTERM);

        sleep(2);
	Log::notice("Finalizando procesos. Enviando SIGKILL...");
        for(std::list<Process>::iterator it = processes.begin(); it != processes.end(); ++it)
           it->send_signal(SIGKILL);

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
