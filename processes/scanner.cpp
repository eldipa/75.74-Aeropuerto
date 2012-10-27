#include <iostream>
#include <stdlib.h>
#include <unistd.h>

#include "api_scanner.h"
#include "api_cinta.h"

int main(int argc, char *argv[]) {

   if (argc < 4) {
      exit(1);
   }

   ApiScanner api(atoi(argv[1]));
   ApiCinta api_cinta_in(atoi(argv[2]));
   ApiCinta api_cinta_out(atoi(argv[3]));
   
   for(;;) {
      bool sospechoso = (rand() % 100) == 0;

      Equipaje equipaje = api_cinta_in.sacar_equipaje();

      if (sospechoso) {
         api.equipaje_sospechoso(equipaje);
      } else {
         api.equipaje_limpio(equipaje);
      }

      sleep(rand() % 10);
      api_cinta_out.poner_equipaje(equipaje);
   }
}
