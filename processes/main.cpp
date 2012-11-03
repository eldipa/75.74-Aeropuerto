#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>

#include "log.h"
#include "process.h"
#include "equipaje.h"
#include "constants.h"

char *args_robot_checkin[] = { (char*) "robot_checkin",
                               (char*)"0",  // ID
                               (char*) TOSTRING(MAKE_PATH) "/mem.txt",
                               (char*) TOSTRING(MAKE_PATH) "/sem.txt",
                               (char*) TOSTRING(MAKE_PATH) "/sem.txt",
                               (char*) TOSTRING(MAKE_PATH) "/sem.txt",
                               NULL };

char *args_puesto_checkin[] = { (char*) "puesto_checkin",
                                (char*)"0",  // ID
                                (char*) TOSTRING(MAKE_PATH) "/mem.txt",
                                (char*) TOSTRING(MAKE_PATH) "/sem.txt",
                                NULL };
/*
 * Crea un puesto de checkin comunicado con un robot_scanner a travez de una cinta.
 **/

int main() {
  CintaCheckin checkin_to_scanner(TOSTRING(MAKE_PATH) "/mem.txt",
                                  TOSTRING(MAKE_PATH) "/sem.txt",
                                  true);


  Log::info("iniciando simulación...");

  Process puesto_checkin("puesto_checkin", args_puesto_checkin);
  Process robot_checkin("robot_checkin", args_robot_checkin);
   	
  sleep( TIEMPO_SIMULACION );

  Log::info("finalizando robots...");

  puesto_checkin.send_signal(SIGTERM, false);
  robot_checkin.send_signal(SIGTERM, false);

  Log::info("finalizando simulación...");

  return 0;
}
