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
#include <syslog.h>

int main( int argc, char** argv ) {
   
   if (argc < 3) {
      syslog(LOG_CRIT, "[Crit] Insuficientes parametros para robot de carga, se esperaba (id, cinta_in)\n");
      exit(1);
   }

   ApiZona zona(atoi(argv[1]));
   ApiCinta cinta_in(atoi(argv[2]));
   ApiAdmin admin;

   syslog(LOG_INFO, "[Info] Iniciando robot carga(%s), %s\n", argv[1], argv[2]);

   for(;;) {
      sleep(rand() % 10);
      syslog(LOG_INFO, "[Info] RobotCarga(%s) Sacando un nuevo equipaje de cinta(%s)\n", argv[1], argv[2]);

      Equipaje equipaje = cinta_in.sacar_equipaje();
      syslog(LOG_INFO, "[Info] RobotCarga(%s) se tomo el equipaje %s\n", argv[1], equipaje.toString().c_str());

      std::string escala = admin.get_escala( equipaje );
      bool equipaje_en_contenedor = false;

      while( !equipaje_en_contenedor ) {
         sleep(rand() % 10);
         syslog(LOG_INFO, "[Info] RobotCarga(%s) esperando contenedor para escala %s\n", argv[1], escala.c_str());
         Contenedor contenedor = zona.esperar_contenedor(escala);

         if( contenedor.espacio_disponible(equipaje) ) {
            syslog(LOG_INFO, "[Info] RobotCarga(%s) agregando equipaje.\n", argv[1]);
            contenedor.agregar_equipaje(equipaje);
            equipaje_en_contenedor = true;
         } else {
            syslog(LOG_INFO, "[Info] RobotCarga(%s) retirando contenedor lleno.\n", argv[1]);
            zona.retirar_contenedor(escala);
         }         
      }

      //api_admin.registrar_equipaje_en_contenedor( e );
   }
 
}
