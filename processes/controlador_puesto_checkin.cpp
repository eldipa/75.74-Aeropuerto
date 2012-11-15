#include "api_checkin.h"
#include "api_constants.h"

#include <stdlib.h>

int main(int argc, char *argv[]) {
   if (argc < 3) {
      Log::crit("Insuficientes parametros para controlador puesto_checkin, se esperaba (id_checkin, num_cinta)\n");
      return (1);
   }

   Log::info("ControladorPuestoCheckin(%d) iniciando controlador puesto de checkin\n", atoi(argv[1]));

   ApiCheckIn api_checkin(atoi(argv[1]), PATH_KEYS, atoi(argv[2]));

   for(;;) {
      tMeansajeCheckin msg;
      api_checkin.recibir_mensaje_controlador_checkin(msg);

      if( msg.iniciar_checkin ) {
         Log::info("ControladorPuestoCheckin(%d) llego mensaje puesto_checkin %d iniciar_checkin num_vuelo %d\n", atoi(argv[1]), msg.mtype, msg.num_vuelo);
         api_checkin.iniciar_checkin(msg.num_vuelo);
      } else {         
         Log::info("ControladorPuestoCheckin(%d) llego mensaje puesto_checkin %d cerrar_checkin \n", atoi(argv[1]), msg.mtype);
         api_checkin.cerrar_checkin();
      }
   }


}
