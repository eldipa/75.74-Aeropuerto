#include <iostream>
#include <stdlib.h>
#include <unistd.h>

#include <vector>
#include <string>

#include "api_cinta.h"
#include "api_zona.h"
#include "api_admin.h"

#include "contenedor.h"
#include "equipaje.h"

int main( int argc, char** argv ) {
   
   if (argc < 3) {
      exit(1);
   }

   ApiZona zona(atoi(argv[1]));
   ApiCinta cinta_in(atoi(argv[2]));
   ApiAdmin admin;

   for(;;) {
      Equipaje equipaje = cinta_in.sacar_equipaje();
      std::string escala = admin.get_escala( equipaje );
      bool equipaje_en_contenedor = false;

      while( !equipaje_en_contenedor ) {
         Contenedor contenedor = zona.esperar_contenedor(escala);

         if( contenedor.espacio_disponible(equipaje) ) {
            contenedor.agregar_equipaje(equipaje);
            equipaje_en_contenedor = true;
         } else {
            zona.retirar_contenedor(escala);
         }         
      }

      //api_admin.registrar_equipaje_en_contenedor( e );
   }
 
}
