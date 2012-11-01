#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "equipaje.h"
#include "log.h"
#include "cinta.h"
#include "constants.h"

int main(int argc, char *argv[]) {
   int next_rfid = 1;

   if (argc < 4) {
      Log::crit("Insuficientes parametros para puesto_checkin, se esperaba (id, cinta_out_path1 cinta_out_path2)\n");
      return (1);
   }

   Cinta<Equipaje, CAPACIDAD_CINTA_CHECKIN>  cinta_checkin_out( PUESTOS_CHECKIN_POR_CINTA,
                                                                ROBOT_CHECKIN_POR_CINTA,
                                                                argv[2],
                                                                argv[3]);
   
   Log::info("Iniciando puesto_checkin(%s), %s \n", argv[1], argv[2] );
   
   for(;;) {
      Equipaje equipaje( next_rfid );
      Log::info("Puesto Checkin(%s) llego  equipaje (rfid=%d).Intento ponerlo en la cinta \n", argv[1], next_rfid);

      cinta_checkin_out.poner_equipaje( equipaje, atoi(argv[1]) );
      Log::info("Puesto Checkin(%s) equipaje (rfid=%d) llendo a robot_checkin \n", argv[1], next_rfid);

      next_rfid++;
      sleep(rand() % SLEEP_PUESTO_CHECKIN);
   }

}
