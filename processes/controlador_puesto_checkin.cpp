#include "api_checkin.h"
#include "api_constants.h"
#include "api_carga.h"
#include "mensajes.h"

#include <stdlib.h>
#include <cstdio>

int obtener_cantidad_equipaje_intercargo(int numero_de_vuelo, const char * file_path){
	int cantidad_equipajes = 0;
	char primera_linea[MAX_PATH_SIZE];
	FILE * equipajes;
	int vuelo_destino;
	int total_equipajes;
	
	equipajes = fopen(file_path,"rt");
	fscanf(equipajes,"%s",primera_linea);
	while(fscanf(equipajes,"%d:%d",&vuelo_destino, &total_equipajes) != EOF){
			if(vuelo_destino == numero_de_vuelo){
				cantidad_equipajes = total_equipajes;
				break;
			}
	}
	fclose(equipajes);
	
	return cantidad_equipajes;
}

int main(int argc, char *argv[]) try {
	int cant_equipaje_intercargo, cant_equipaje_checkin;
	
   if (argc < 3) {
      Log::crit("Insuficientes parametros para controlador puesto_checkin, se esperaba (directorio_de_trabajo, config_file, id_checkin, archivo_equipajes_intercargo)\n");
      return (1);
   }

   Log::info("Iniciando controlador puesto de checkin %d %s %s %s\n", argc, argv[1], argv[3], argv[4]);

   ApiCheckIn api_checkin(argv[1],argv[2], atoi(argv[3]));
   cant_equipaje_intercargo = cant_equipaje_checkin = 0;

   for(;;) {
      tMeansajeCheckin msg;
      api_checkin.recibir_mensaje_controlador_checkin(msg);

      if( msg.iniciar_checkin ) {
         Log::info("Llego mensaje puesto_checkin %d iniciar_checkin num_vuelo %d\n", msg.mtype, msg.num_vuelo);
         api_checkin.iniciar_checkin(msg.num_vuelo);
         cant_equipaje_intercargo = obtener_cantidad_equipaje_intercargo(msg.num_vuelo, argv[4]);
      } else {
         Log::info("Llego mensaje puesto_checkin %d cerrar_checkin \n",  msg.mtype);
         cant_equipaje_checkin = api_checkin.cerrar_checkin();

         Log::info("Cierro checkin, aviso a robot_carga que total_equipajes=%d(checkin) + %d(intercargo) = %d \n", 
                   cant_equipaje_checkin, cant_equipaje_intercargo, cant_equipaje_checkin+cant_equipaje_intercargo);

         ApiCarga api_carga(argv[1],argv[2], msg.num_zona,  msg.num_zona);
         api_carga.cargar_equipajes( cant_equipaje_checkin+cant_equipaje_intercargo );
      }
   }

} catch(const std::exception &e) {
   Log::crit("%s", e.what());
} catch(...) {
   Log::crit("Critical error. Unknow exception at the end of the 'main' function.");
}
