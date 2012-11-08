#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "equipaje.h"
#include "constants.h"

#include "api_checkin.h"
#include "log.h"

int main(int argc, char *argv[]) {
   int next_rfid = 10;

   if (argc < 4) {
      Log::crit("Insuficientes parametros para puesto_checkin, se esperaba (id, path_cinta num_cinta)\n");
      return (1);
   }

   ApiCheckIn checkin(atoi(argv[1]), argv[2], atoi(argv[3]));

   Log::info("Iniciando puesto_checkin(%s), conectado a cinta %i\n", argv[1], atoi(argv[3]) );

   for(int vuelo = 3; vuelo < 5; vuelo ++) {

      Log::info("Iniciando checkin del vuelo(%i)\n", vuelo );
      sleep(rand() % SLEEP_PUESTO_CHECKIN);
      //checkin.iniciar_checkin(vuelo);

      for (int i = 0; i < 5; i ++ ) {

         Equipaje equipaje( next_rfid , rand() % TOPE_PESO_VALIJA);
         Log::info("Puesto Checkin(%s) llego equipaje (rfid=%d)\n", argv[1], next_rfid);

         sleep(rand() % SLEEP_PUESTO_CHECKIN);

         checkin.registrar_equipaje(equipaje);

         Log::info("Puesto Checkin(%s) enviando (rfid=%d) a robot_checkin\n", argv[1], next_rfid);

         next_rfid++;
         sleep(rand() % SLEEP_PUESTO_CHECKIN);

      }

      Log::info("Terminando checkin del vuelo(%i)\n", vuelo );
      sleep(rand() % SLEEP_PUESTO_CHECKIN);
      //checkin.cerrar_checkin();
   }

}
