#include "api_checkin.h"
#include "api_constants.h"
#include "api_carga.h"

#include <stdlib.h>

int main(int argc, char *argv[]) {
	char path[300];
	char path_cola[300];

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
         int equipajes = api_checkin.cerrar_checkin();
         Log::info("ControladorPuestoCheckin(%d) cierro checkin, avisoa  robot_carga que total_equipajes=%d \n", atoi(argv[1]), equipajes);

         strcpy(path, PATH_KEYS);
         strcat(path, PATH_CINTA_CONTENEDOR);
         strcpy(path_cola, PATH_KEYS);
         strcat(path_cola, PATH_COLA_ROBOTS_ZONA_TRACTORES);
         ApiCarga api_carga(1, path, 1, path_cola);
         api_carga.llegaron_todos_los_equipajes(equipajes);
      }
   }


}
