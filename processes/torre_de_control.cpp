#include "api_torre_de_control.h"

#include "log.h"

#include "api_constants.h"
#include <stdlib.h>
#include <cstring>

int main( /*int argc, char** argv */) {
	char path_torre_control[300];

   /*if (argc < 2) {
      Log::crit("Insuficientes parametros para torre de control, se esperaba ()\n");
      exit(1);
   }*/

	strcpy(path_torre_control, PATH_KEYS);
	strcat(path_torre_control, PATH_TORRE_DE_CONTROL);

   Log::info("Iniciando la torre de control");
   ApiTorreDeControl torre_de_control(path_torre_control);

   // Acá tendría que ir la lógica de la torre de control
   // Por ahora harcodeo una notificación de llegada de vuelo
   if (true) {
	   Log::info("Notificando llegada del vuelo 3");
	   torre_de_control.notificar_llegada_vuelo(3);
   }

   Log::info("Nada mas que hacer");
   return 0;
}
