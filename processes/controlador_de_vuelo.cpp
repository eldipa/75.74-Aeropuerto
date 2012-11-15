#include <stdlib.h>

#include "log.h"
#include "api_checkin.h"
#include "api_controlador_de_vuelo.h"

#include "process.h"
#include "constants.h"

int tomar_zona(int num_zona);
int get_duracion_checkin(int num_vuelo);

class VueloNoRegistradoException {
public:
	VueloNoRegistradoException(int num_vuelo) :
			num_vuelo(num_vuelo) {
	}
private:
	int num_vuelo;
};

void run_generador_pasajeros() {

	static char *args_generador_pasajeros[] = { (char*) "generador_pasajeros", (char*) "1", //num_vuelo
			(char*) "1", (char*) "1", NULL };
	Process generator("generador_pasajeros", args_generador_pasajeros);
}

int main(int argc, char** argv) {
   if (argc < 2) {
      Log::crit("Insuficientes parametros para controlador de vuelo (num_vuelo)\n");
      return (1);
   }

   int num_vuelo = atoi(argv[1]);
   int  zona_utilizada,  duracion_checkin, num_puesto_checkin;
   ApiControladorDeVuelo api_vuelo(PATH_KEYS, num_vuelo);
   
   num_puesto_checkin = 1;

   try {
      sleep(SLEEP_DESPACHO_VUELO);
      Log::info("ControladorDeVuelos(%d) Iniciando despachante de vuelos...", num_vuelo);

      //activo robot_despacho
      zona_utilizada = tomar_zona(num_vuelo);
      Log::info("ControladorDeVuelos(%d) robot de despacho empieza a recibir equipajes para zona %d", num_vuelo, zona_utilizada);
      
      //abro checkin
      api_vuelo.iniciar_checkin(num_puesto_checkin);
      Log::info("ControladorDeVuelos(%d) incicio checkin puesto %d", num_vuelo, 1);
      run_generador_pasajeros();

      //espero cierre checkin
      duracion_checkin = get_duracion_checkin(num_vuelo);
      sleep(duracion_checkin);
      api_vuelo.cerrar_checkin(num_puesto_checkin);
      Log::info("ControladorDeVuelos(%d) cierro checkin %d", num_vuelo);

   } catch (VueloNoRegistradoException) {
      Log::crit("El vuelo %d no se encuentra registrado en la BD\n", num_vuelo);
      return 1;
   }
}

int tomar_zona(int num_vuelo) {
	return num_vuelo;
}

int get_duracion_checkin(int num_vuelo) {
	if (num_vuelo >= 0)
		return SLEEP_DURACION_CHECKIN;
	else
		return SLEEP_DURACION_CHECKIN;

}
