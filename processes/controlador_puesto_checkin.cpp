#include "api_checkin.h"
#include "api_constants.h"
#include "api_carga.h"
#include "mensajes.h"

#include <stdlib.h>

int main(int argc, char *argv[]) {
	char path[300];

   if (argc < 2) {
      Log::crit("Insuficientes parametros para controlador puesto_checkin, se esperaba (id_checkin)\n");
      return (1);
   }

   Log::info("ControladorPuestoCheckin(%d) iniciando controlador puesto de checkin\n", atoi(argv[1]));

   ApiCheckIn api_checkin(atoi(argv[1]), PATH_KEYS);

   for(;;) {
      tMeansajeCheckin msg;
      api_checkin.recibir_mensaje_controlador_checkin(msg);

      if( msg.iniciar_checkin ) {
         Log::info("ControladorPuestoCheckin(%d) llego mensaje puesto_checkin %d iniciar_checkin num_vuelo %d\n", atoi(argv[1]), msg.mtype, msg.num_vuelo);
         api_checkin.iniciar_checkin(msg.num_vuelo);
      } else {         
         Log::info("ControladorPuestoCheckin(%d) llego mensaje puesto_checkin %d cerrar_checkin \n", atoi(argv[1]), msg.mtype);
         int equipajes = api_checkin.cerrar_checkin();
         Log::info("ControladorPuestoCheckin(%d) cierro checkin, avisoa  robot_carga que total_equipajes=%d \n", atoi(argv[1]), equipajes);

         MENSAJE_CHECKIN_CERRADO mensaje;
         mensaje.checkin_cerrado = 1;
         mensaje.cantidad_equipaje_total = equipajes;

         strcpy(path, PATH_KEYS);
         strcat(path, PATH_COLA_CONTROL_CARGA_CHECKIN);
         MessageQueue cola_mensajes_con_despachante_de_vuelo(path, 1);
         cola_mensajes_con_despachante_de_vuelo.push(&mensaje, sizeof(MENSAJE_CHECKIN_CERRADO)-sizeof(long));

      }
   }


}
