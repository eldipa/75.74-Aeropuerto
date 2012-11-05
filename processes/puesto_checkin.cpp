#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "equipaje.h"
#include "log.h"
#include "constants.h"

int main(int argc, char *argv[]) {
   int next_rfid = 10;

   if (argc < 4) {
      Log::crit("Insuficientes parametros para puesto_checkin, se esperaba (id, path_cinta num_cinta)\n");
      return (1);
   }

   Log::info("Iniciando puesto_checkin(%s), conectado a cinta %i\n", argv[1], atoi(argv[3]) );

   CintaCheckin cinta_checkin_out(argv[2], atoi(argv[3]));

   for(;;) {
      Equipaje equipaje( next_rfid , rand() % TOPE_PESO_VALIJA);
      Log::info("Puesto Checkin(%s) llego equipaje (rfid=%d)\n", argv[1], next_rfid);

      sleep(rand() % SLEEP_PUESTO_CHECKIN);

      Log::info("Puesto Checkin(%s) enviando (rfid=%d) a robot_checkin\n", argv[1], next_rfid);
      cinta_checkin_out.poner_equipaje( equipaje );

      next_rfid++;
      sleep(rand() % SLEEP_PUESTO_CHECKIN);

   }

}
