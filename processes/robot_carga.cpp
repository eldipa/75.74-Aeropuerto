#include <iostream>
#include <stdlib.h>
#include <unistd.h>

#include <map>
#include <string>

#include "api_carga.h"
#include "contenedor.h"
#include "equipaje.h"
#include "log.h"
#include "cintas.h"
#include "api_admincontenedores.h"

int main( int argc, char** argv ) {
   
   if (argc < 6) {
      Log::crit("Insuficientes parametros para robot de carga, se esperaba (id, path_cinta_contenedor, id_cinta_contenedor, path_controlador_carga, path_admin_contenedores)\n");
      exit(1);
   }

   int equipajes_por_cagar , equipajes_cargados;
   equipajes_por_cagar = equipajes_cargados = 0;

   std::map<std::string, Contenedor> contenedores_por_escala;

   CintaContenedor cinta_contenedor( argv[2], atoi(argv[3]));
   ApiCarga api_carga(argv[4], atoi(argv[1]));
   ApiAdminContenedores api_admin_contenedores(argv[5]);

   Log::info("Iniciando robot carga(%s), %s\n", argv[1], argv[2]);

   for(;;) {

      Log::info("RobotCarga(%s) Bloqueo esperando orden del controlador de carga\n", argv[1]);      
      equipajes_por_cagar = api_carga.get_equipajes_por_cargar();
      Log::info("RobotCarga(%s) Llego orden de carga, cargar %d equipajes\n", argv[1], equipajes_por_cagar);      

      equipajes_cargados = 0;

      while( equipajes_cargados < equipajes_por_cagar ) {

         sleep(rand() % 10);
         Log::info("RobotCarga(%s) Intentando tomar un nuevo equipaje de cinta(%s)\n", argv[1], argv[2]);

         Equipaje equipaje = cinta_contenedor.sacar_equipaje();
         Log::info("RobotCarga(%s) se tomo el equipaje %s con escala destino %s\n", 
                   argv[1], equipaje.toString().c_str(), equipaje.getRfid().escala_destino.c_str());


         if( equipaje.peso() <= MAX_PESO_CONTENEDOR ) {
            std::string escala = equipaje.getRfid().escala_destino;
            
            if(contenedores_por_escala.find(escala) == contenedores_por_escala.end()) {
               Log::crit("RobotCarga(%s) no tengo contenedor, pido contenedor para escala %s\n", 
                         argv[1], escala.c_str() );
               api_admin_contenedores.pedir_contenedor();
               contenedores_por_escala.insert(std::pair<std::string,Contenedor>(escala,Contenedor()));
            }

            if(!contenedores_por_escala[escala].hay_lugar(equipaje)) {
               Log::crit("RobotCarga(%s) contenedor lleno, pido contenedor para escala %s\n", 
                         argv[1], escala.c_str() );
               api_carga.contenedor_cargado(contenedores_por_escala[escala]);
               api_admin_contenedores.pedir_contenedor();
               contenedores_por_escala[escala] = Contenedor();            
            }

            Log::crit("RobotCarga(%s) pongo equipaje %s en contenedor de escala %s\n", 
                      argv[1], equipaje.toString().c_str(), escala.c_str() );
            contenedores_por_escala[escala].agregar_equipaje(equipaje);
            equipajes_cargados++;

         } else {
            Log::crit("RobotCarga(%s) El equipaje %ses mas grande que el propio contenedor!!!\n", 
                      argv[1], equipaje.toString().c_str() );
         }
      }

      // cargo el resto de los contenedores
      std::map<std::string, Contenedor>::iterator it;
      for(it=contenedores_por_escala.begin();it!=contenedores_por_escala.end();it++)
         api_carga.contenedor_cargado((*it).second);
      contenedores_por_escala.clear();

      Log::info("RobotCarga(%s) fin de carga de equipajes del vuelo\n", argv[1]);      
   }
 
}
