#include <iostream>
#include <stdlib.h>
#include <unistd.h>

#include "api_cinta.h"
#include "api_zona.h"
#include "api_admin.h"
#include "api_scanner.h"
#include "log.h"

#include "contenedor.h"
#include "equipaje.h"

#include <map>
#include <string>


/*
 * Argumentos para el robot de despacho.
 * ./robot_despacho <id_robot> <cinta_in> <cinta_zona1> <zona1> ... <cinta_zonan> <zonan>
 * En donde la cinta i lleva equipajes hacia la zona i.
 **/

int main( int argc, char** argv ) {
   
   if ( (argc < 5) || ((argc-4)%2!=0) ) {
      Log::crit("Insuficientes parametros para robot de despacho, se esperaba (id, cinta_in cinta1 cinta1 zona1 cinta2 zona2...)\n");
      exit(1);
   }

   ApiScanner scanner(0);
   ApiAdmin admin;
   ApiCinta cinta_in(atoi(argv[2]));
   ApiCinta cinta_out(atoi(argv[3]));

   Log::info("Iniciando robot despacho(%s), %s, %s atiende %s zonas\n", argv[1], argv[2], argv[3], (argc-4)/2);

   std::map<int,ApiCinta> salidas;
   for( int i=4; i<argc; i+=2 ) {
      Log::info("Robot despacho(%s), lleva a zona %s a travez de la cinta\n", argv[1], argv[i], argv[i+1] );
      salidas.insert( std::pair<int,ApiCinta>( atoi(argv[i]), ApiCinta(atoi(argv[i+1])) ) ); 
   }
   
   for(;;) {
      sleep(rand() % 10);

      Log::info("Robot de despacho(%s) Sacando un nuevo equipaje de cinta(%s)\n", argv[1], argv[2]);
      Equipaje equipaje = cinta_in.sacar_equipaje();

      int num_vuelo = admin.get_numero_vuelo(equipaje);
      int num_zona = admin.get_numero_zona(num_vuelo);
      Log::info("Robot despacho(%s), toma el equipaje %s con destino al vuelo %d (zona %d)", 
                argv[1], 
                equipaje.toString().c_str(), 
                num_vuelo,
                num_zona );

      if( scanner.es_sospechoso(equipaje) ) {
         cinta_out.poner_equipaje( equipaje );
         Log::info("Robot de despacho(%s) el equipaje %s fue marcado como sospechoso.vuelvo a colocarlo en la cinta, %s\n", 
                   argv[1], equipaje.toString().c_str(), argv[3]);
      } else if ( salidas.find(num_zona) == salidas.end() ) {
         cinta_out.poner_equipaje( equipaje );
         Log::info("Robot de despacho(%s) el equipaje %s no pertenece a mis zonas, cuelvo a colocarlo en la cinta %s\n", 
                   argv[1], equipaje.toString().c_str(), argv[3] );
      } else {
         ApiCinta& cinta = salidas.find(num_zona)->second;
         cinta.poner_equipaje(equipaje);
         Log::info("Robot de despacho(%s)  pongo equipaje %s en la cinta %d con destino a la zona %d\n", 
                   argv[1], cinta.get_numero(), num_zona); 
      }
         
   } 
 
}
