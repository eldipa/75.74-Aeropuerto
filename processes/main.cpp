#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>

#include "log.h"
#include "process.h"
#include "equipaje.h"
#include "constants.h"
<<<<<<< HEAD
#include "daemon.h"
=======
#include "cintaprincipal.h"

>>>>>>> modificamos los ipcs para que reciban el abs_path y el proj_id.modifico los constructores de las cintas.reciben un solo path y un num_cinta

char *args_robot_checkin[] = { (char*) "robot_checkin",
                               (char*)"0",  // ID
                               (char*) TOSTRING(MAKE_PATH) PATH_CINTA_CHECKIN,
                               (char*)"0",
                               (char*) TOSTRING(MAKE_PATH) PATH_CINTA_SCANNER,
                               (char*)"0",
                               NULL };

char *args_puesto_checkin[] = { (char*) "puesto_checkin",
                                (char*)"0",  // ID
                                (char*) TOSTRING(MAKE_PATH) PATH_CINTA_CHECKIN,
                                (char*)"0",
                                NULL };
/*
 * Crea un puesto de checkin comunicado con un robot_scanner a travez de una cinta.
 **/

int main() {
	pid_t pid = 0;

  CintaCheckin checkin_to_scanner(TOSTRING(MAKE_PATH) PATH_CINTA_CHECKIN,
                                  0 ,
                                  true);

  CintaCentral cinta_principal( true, TOSTRING(MAKE_PATH) PATH_CINTA_CENTRAL, 0);

  Log::info("iniciando simulación...");

  Process puesto_checkin("puesto_checkin", args_puesto_checkin);
  Process robot_checkin("robot_checkin", args_robot_checkin);
  
  be_a_daemon();
   	
  sleep( TIEMPO_SIMULACION );

  Log::info("finalizando robots...");

  puesto_checkin.send_signal(SIGTERM, false);
  robot_checkin.send_signal(SIGTERM, false);

  Log::info("finalizando simulación...");

  return 0;
}
