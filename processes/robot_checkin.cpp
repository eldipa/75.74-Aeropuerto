#include <iostream>
#include <stdlib.h>
#include <unistd.h>

#include "api_cinta.h"
#include "log.h"
#include "equipaje.h"

#include <vector>
#include <string>
#include "constants.h"

int main( int argc, char** argv ) {

   if ( argc < 6 ) {
      Log::crit("Insuficientes parametros para robot de checkin, se esperaba (id, path_cinta_checkin num_cinta_checkin_in path_cinta_scaner num_cinta_scanner)\n");
      exit(1);
   }

   CintaCheckin cinta_checkin( argv[2], atoi(argv[3]) );
   CintaScanner cinta_scanner( argv[4], atoi(argv[5]) );

   // cada robot de checkin distribuye entre n scanners
   for(;;) {
      Equipaje equipaje = cinta_checkin.sacar_equipaje();
      Log::info( "Robot de checkin(%s), saco equipaje %s y lo coloco en cinta de scanner",
                 argv[1], equipaje.toString().c_str());

      sleep(rand() % SLEEP_ROBOT_CHECKIN);

      cinta_scanner.poner_equipaje( equipaje );
   }
 }
