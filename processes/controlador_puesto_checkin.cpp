#include "api_checkin.h"
#include "api_constants.h"
#include "api_carga.h"
#include "mensajes.h"

#include <stdlib.h>

int main(int argc, char *argv[]) try {
	char path[300];

   if (argc < 2) {
      Log::crit("Insuficientes parametros para controlador puesto_checkin, se esperaba (directorio_de_trabajo, id_checkin)\n");
      return (1);
   }

   Log::info("Iniciando controlador puesto de checkin\n", atoi(argv[2]));

   ApiCheckIn api_checkin(argv[1],atoi(argv[2]));

   for(;;) {
      tMeansajeCheckin msg;
      api_checkin.recibir_mensaje_controlador_checkin(msg);

      if( msg.iniciar_checkin ) {
         Log::info("Llego mensaje puesto_checkin %d iniciar_checkin num_vuelo %d\n", atoi(argv[2]), msg.mtype, msg.num_vuelo);
         api_checkin.iniciar_checkin(msg.num_vuelo);
      } else {
         Log::info("Llego mensaje puesto_checkin %d cerrar_checkin \n", atoi(argv[2]), msg.mtype);
         int equipajes = api_checkin.cerrar_checkin();
         Log::info("Cierro checkin, aviso a robot_carga que total_equipajes=%d \n", atoi(argv[2]), equipajes);

         MENSAJE_CHECKIN_CERRADO mensaje;
         mensaje.checkin_cerrado = 1;
         mensaje.cantidad_equipaje_total = equipajes; // FALTA SUMAR EQUIPAJES POR INTERCARGO

         strcpy(path, PATH_KEYS);
         strcat(path, PATH_COLA_CONTROL_CARGA_CHECKIN);
         MessageQueue cola_mensajes_con_despachante_de_vuelo(path, msg.num_zona);
         cola_mensajes_con_despachante_de_vuelo.push(&mensaje, sizeof(MENSAJE_CHECKIN_CERRADO)-sizeof(long));

      }
   }

} catch(const std::exception &e) {
   Log::crit("%s", e.what());
} catch(...) {
   Log::crit("Critical error. Unknow exception at the end of the 'main' function.");
}
