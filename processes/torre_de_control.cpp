#include "api_torre_de_control.h"

#include "log.h"

#include <stdlib.h>

int main( int argc, char** argv ) {

   if (argc < 2) {
      Log::crit("Insuficientes parametros para torre de control, se esperaba (path_torre_control)\n");
      exit(1);
   }

   Log::info("Iniciando la torre de control");
   ApiTorreDeControl torre_de_control(argv[1]);

   // Acá tendría que ir la lógica de la torre de control
   // Por ahora harcodeo una notificación de llegada de vuelo
   if (true) {
	   Log::info("Notificando llegada del vuelo 3");
	   torre_de_control.notificar_llegada_vuelo(3);
   }

   Log::info("Nada mas que hacer");
   return 0;
}