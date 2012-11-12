#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>

#include "log.h"

#include "process.h"
#include "constants.h"
#include "messagequeue.h"
#include "daemon.h"
#include "conexiones_aeropuerto.h"

char *args_robot_checkin[] = { (char*) "robot_checkin",
		(char*) "0", // ID
		(char*) TOSTRING(MAKE_PATH) PATH_CINTA_CHECKIN, (char*) "0",
		(char*) TOSTRING(MAKE_PATH) PATH_CINTA_SCANNER, (char*) "0", NULL };

char *args_scanner[] = { (char*) "robot_scanner",
		(char*) "1", // ID
		(char*) TOSTRING(MAKE_PATH) PATH_CINTA_SCANNER, (char*) "0",
		(char*) TOSTRING(MAKE_PATH) PATH_CINTA_CENTRAL, (char*) "0", NULL };

char *args_robot_despacho[] = { (char*) "robot_despacho",
		(char*) "1", // ID
		(char*) TOSTRING(MAKE_PATH) PATH_CINTA_CENTRAL, (char*) "0",
		(char*) TOSTRING(MAKE_PATH) PATH_CINTA_CONTENEDOR, (char*) "0", (char*) "0", (char*) "1",
		NULL };

char *args_robot_carga0[] = { (char*) "robot_carga",
		(char*) "0", // ID
		(char*) TOSTRING(MAKE_PATH) PATH_CINTA_CONTENEDOR, (char*) "0",
		(char*) TOSTRING(MAKE_PATH) PATH_CONTROLADOR_DE_CARGA,
		(char*) TOSTRING(MAKE_PATH) PATH_ADMIN_CONTENEDORES, (char*) TOSTRING(MAKE_PATH)
		PATH_COLA_ROBOTS_ZONA_TRACTORES, NULL };

char *args_robot_carga1[] = { (char*) "robot_carga",
		(char*) "1", // ID
		(char*) TOSTRING(MAKE_PATH)PATH_CINTA_CONTENEDOR, (char*) "1",
		(char*) TOSTRING(MAKE_PATH)PATH_CONTROLADOR_DE_CARGA,
		(char*) TOSTRING(MAKE_PATH)PATH_ADMIN_CONTENEDORES, (char*) TOSTRING(MAKE_PATH)
		PATH_COLA_ROBOTS_ZONA_TRACTORES, NULL };

char *args_controlador_de_carga[] = { (char*) "controlador_de_carga",
		(char*) TOSTRING(MAKE_PATH) PATH_CONTROLADOR_DE_CARGA, (char*) "0", //ID robot_carga
		(char*) "0", //num_vuelo
		(char*) TOSTRING(MAKE_PATH)
		PATH_COLA_ROBOTS_ZONA_TRACTORES, NULL };

char *args_robot_sospechosos[] = { (char*) "robot_control_equipaje_sospechoso", (char*) "2",
		(char*) "2", // ID
		(char*) TOSTRING(MAKE_PATH)
		PATH_CINTA_CENTRAL, (char*) "0", NULL };

char *args_robot_intercargo[] = { (char*) "robot_intercargo",
		(char*) "0", // ID
		(char*) TOSTRING(MAKE_PATH) PATH_TORRE_DE_CONTROL,
		(char*) TOSTRING(MAKE_PATH) PATH_CINTA_CENTRAL, (char*) "0", NULL };

char *args_torre_de_control[] = { (char*) "torre_de_control",
		(char*) TOSTRING(MAKE_PATH) PATH_TORRE_DE_CONTROL, NULL };

char *args_tractor1[] = { (char*) "tractor", (char*) "1",
		(char*) TOSTRING(MAKE_PATH)PATH_COLA_ROBOTS_ZONA_TRACTORES,
		(char*) TOSTRING(MAKE_PATH) PATH_COLA_TRACTORES_AVIONES, NULL };

char *args_tractor2[] = { (char*) "tractor", (char*) "2",
		(char*) TOSTRING(MAKE_PATH)PATH_COLA_ROBOTS_ZONA_TRACTORES,
		(char*) TOSTRING(MAKE_PATH) PATH_COLA_TRACTORES_AVIONES, NULL };

char *args_avion1[] = { (char*) "avion", (char*) "1",
		(char*) TOSTRING(MAKE_PATH) PATH_COLA_TRACTORES_AVIONES, NULL };

char *args_avion2[] = { (char*) "avion", (char*) "2",
		(char*) TOSTRING(MAKE_PATH) PATH_COLA_TRACTORES_AVIONES, NULL };

/*
 * Crea un puesto de checkin comunicado con un robot_scanner a travez de una cinta.
 **/

//Dummy signal handler
void dummy(int) {
}

int main(int argc, char **argv) {
//be_a_daemon();

//Ignoring the Interrupt Signal
//In fact, the signal is catched but its handler is useless.
	struct sigaction ignore;
	memset(&ignore, 0, sizeof(struct sigaction));
	ignore.sa_handler = &dummy;

	sigaction(SIGINT, &ignore, 0);

	char *path;
	if (argc < 2)
		path = (char*) TOSTRING(MAKE_PATH);
	else
		path = argv[1];

	Log::info("Creando aeropuerto...");

	ConexionesAeropuerto aeropuerto(path);

	Log::info("iniciando simulación...");

	static char *args_puesto_checkin[] = { (char*) "puesto_checkin", (char*) "0", // ID
			path, (char*) "0", NULL };

	Process puesto_checkin("puesto_checkin", args_puesto_checkin);
	Process robot_checkin("robot_checkin", args_robot_checkin);
	Process robot_scanner("robot_scanner", args_scanner);
	Process robot_despacho("robot_despacho", args_robot_despacho);
	Process robot_carga0("robot_carga", args_robot_carga0);
	Process robot_carga1("robot_carga", args_robot_carga1);
	//Process controlador_de_carga("controlador_de_carga", args_controlador_de_carga);
	Process robot_sospechosos("robot_control_equipaje_sospechoso", args_robot_sospechosos);
	Process robot_intercargo("robot_intercargo", args_robot_intercargo);
	Process torre_de_control("torre_de_control", args_torre_de_control);
	Process tractor1("tractor", args_tractor1);
	Process tractor2("tractor", args_tractor2);
	Process avion1("avion", args_avion1);
	Process avion2("avion", args_avion2);

	Log::info("Done, waiting for a SIGINT signal.");
	pause();
	Log::info("Signal recieved. Shutdown...");

	Log::info("finalizando robots...");

	puesto_checkin.send_signal(SIGTERM);
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
}
