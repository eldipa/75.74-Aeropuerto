#include <iostream>
#include <stdlib.h>
#include <unistd.h>

#include "api_cinta.h"
#include "log.h"
#include "equipaje.h"

#include <vector>
#include <string>


int main( int argc, char** argv ) {
   
   if ( argc < 3 ) {
      Log::crit("Insuficientes parametros para robot de checkin, se esperaba (id, cinta_checkin_in cinta_scanner_out_1...cinta_scanner_out_n)\n");
      exit(1);
   }

   ApiCinta cinta_checkin_in( atoi(argv[1]) );

   std::vector<ApiCinta> cintas_a_scanner;
   for( int i=2; i<argc; i++ ) {
      cintas_a_scanner.push_back( ApiCinta(atoi(argv[i])) );
   }

   // round robin entre las cintas de scanner
   int proxima_cinta = 0;
   for(;;) {
      sleep(rand() % 10);
      Equipaje equipaje = cinta_checkin_in.sacar_equipaje();
      Log::info( "Robot de checkin(%s), saco equipaje %s y lo coloco en cinta de scanner %d",
                 argv[1], equipaje.toString().c_str(), proxima_cinta );

      cintas_a_scanner[proxima_cinta].poner_equipaje( equipaje );
      proxima_cinta = (proxima_cinta+1)%cintas_a_scanner.size();
   } 
 }
