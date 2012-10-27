#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "api_scanner.h"
#include "api_cinta.h"
#include <syslog.h>

int main(int argc, char *argv[]) {

   if (argc < 4) {
      syslog(LOG_CRIT, "[Crit] Insuficientes parametros para scanner, se esperaba (id, cinta_in, cinta_out)\n");
      exit(1);
   }

   ApiScanner api(atoi(argv[1]));
   ApiCinta api_cinta_in(atoi(argv[2]));
   ApiCinta api_cinta_out(atoi(argv[3]));
   syslog(LOG_INFO, "[Info] Iniciando scanner(%s), %s, %s\n", argv[1], argv[2], argv[3]);
   
   for(;;) {
      syslog(LOG_INFO, "[Info] Scanner(%s) Sacando un nuevo equipaje de cinta(%s)\n", argv[1], argv[2]);
      Equipaje equipaje = api_cinta_in.sacar_equipaje();

      bool sospechoso = (rand() % 100) == 0;
      sleep(rand() % 10);

      if (sospechoso) {
         syslog(LOG_INFO, "[Info] Scanner(%s) se encontro sospechoso el equipaje %s\n", argv[1], equipaje.toString().c_str());
         api.equipaje_sospechoso(equipaje);
      } else {
         syslog(LOG_INFO, "[Info] Scanner(%s) equipaje limpio: %s\n", argv[1], equipaje.toString().c_str());
         api.equipaje_limpio(equipaje);
      }

      syslog(LOG_INFO, "[Info] Scanner(%s) pasando equipaje a cinta(%s)\n", argv[1], argv[3]);
      api_cinta_out.poner_equipaje(equipaje);
   }

}
