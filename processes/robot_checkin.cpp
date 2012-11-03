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
      Log::crit("Insuficientes parametros para robot de checkin, se esperaba (id, cinta_checkin_in num_cinta_checkin_in cinta_scanner_out num_cinta_scanner_out)\n");
      exit(1);
   }

   CintaCheckin cinta_checkin_in( argv[2], atoi(argv[3]) );
   CintaScanner cinta_checkin_out( argv[4], atoi(argv[5]) );

   // cada robot de checkin distribuye entre n scanners
   for(;;) {
      Equipaje equipaje = cinta_checkin_in.sacar_equipaje();
      Log::info( "Robot de checkin(%s), saco equipaje %s y lo coloco en cinta de scanner",
                 argv[1], equipaje.toString().c_str());

      sleep(rand() % SLEEP_ROBOT_CHECKIN);

      cinta_checkin_out.poner_equipaje( equipaje );
   }
 }
