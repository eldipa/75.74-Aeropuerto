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

char *args_robot_checkin[] = { (char*) "robot_checkin", (char*)"0",  // ID
                               (char*) TOSTRING(MAKE_PATH) PATH_CINTA_CHECKIN, (char*)"0",
                               (char*) TOSTRING(MAKE_PATH) PATH_CINTA_SCANNER, (char*)"0",
                               NULL };

char *args_scanner[] = { (char*) "scanner", (char*)"0",  // ID
                         (char*) TOSTRING(MAKE_PATH) PATH_CINTA_SCANNER, (char*)"0",
                         (char*) TOSTRING(MAKE_PATH) PATH_CINTA_CENTRAL, (char*)"0",
                         NULL };

char *args_robot_despacho[] = {(char*) "robot_despacho", (char*)"0",  // ID
                               (char*) TOSTRING(MAKE_PATH) PATH_CINTA_CENTRAL, (char*)"0",
                               (char*) TOSTRING(MAKE_PATH) PATH_CINTA_CONTENEDOR, (char*)"0",
                               NULL };

char *args_robot_carga[] = {(char*)"robot_carga", (char*)"0",  // ID
                            (char*) TOSTRING(MAKE_PATH) PATH_CINTA_CONTENEDOR, (char*)"0",
                            NULL };

char *args_robot_intercargo[] = {(char*)"robot_intercargo", (char*)"0", // ID
                                 (char*) TOSTRING(MAKE_PATH) PATH_TORRE_DE_CONTROL,
                                 (char*) TOSTRING(MAKE_PATH) PATH_CINTA_CENTRAL, (char*)"0",
                                 NULL };

char *args_torre_de_control[] = {(char*)"torre_de_control",
                                 (char*) TOSTRING(MAKE_PATH) PATH_TORRE_DE_CONTROL,
                                 NULL };
/*
 * Crea un puesto de checkin comunicado con un robot_scanner a travez de una cinta.
 **/

int main(int argc, char **argv) {

  char *path;
  if (argc < 2)
    path = (char*)TOSTRING(MAKE_PATH);
  else
    path = argv[1];

  Log::info("Creando aeropuerto...");

  ConexionesAeropuerto aeropuerto(path);

  Log::info("iniciando simulación...");

  static char *args_puesto_checkin[] = { (char*) "puesto_checkin", (char*)"0",  // ID
                                         path, (char*)"0",
                                         NULL };

  Process puesto_checkin("puesto_checkin", args_puesto_checkin);
  Process robot_checkin("robot_checkin", args_robot_checkin);
  Process scanner("scanner", args_scanner);
  Process robot_despacho("robot_despacho", args_robot_despacho);
  Process robot_carga("robot_carga", args_robot_carga);
  Process robot_intercargo("robot_intercargo", args_robot_intercargo);
  Process torre_de_control("torre_de_control", args_torre_de_control);

  be_a_daemon();

  sleep( TIEMPO_SIMULACION );

  Log::info("finalizando robots...");

  puesto_checkin.send_signal(SIGTERM);
  robot_checkin.send_signal(SIGTERM);
  scanner.send_signal(SIGTERM);
  robot_despacho.send_signal(SIGTERM);
  robot_carga.send_signal(SIGTERM);
  robot_intercargo.send_signal(SIGTERM);
  torre_de_control.send_signal(SIGTERM);

  Log::info("finalizando simulación...");

  return 0;
}
