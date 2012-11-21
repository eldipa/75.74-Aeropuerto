#include <stdlib.h>

#include "log.h"
#include "api_checkin.h"
#include "api_controlador_de_vuelo.h"
#include "api_torre_de_control.h"
#include "api_despachante.h"

#include "process.h"
#include "constants.h"

#include "database.h"
#include "stmt.h"
#include "tupleiter.h"

#include <sstream>

int tomar_zona(int num_zona);
int get_duracion_checkin(int num_vuelo);
void registrar_zona_en_uso(int num_vuelo, int num_zona);

class VueloNoRegistradoException {
public:
	VueloNoRegistradoException(int num_vuelo) :
			num_vuelo(num_vuelo) {
	}
private:
	int num_vuelo;
};


void run_generador_pasajeros(int num_vuelo, int num_puesto_checkin) {
   char buffer_vuelo[6];
   char buffer_puesto[6];

   std::stringstream vuelo;
   vuelo << num_vuelo;
   strcpy(buffer_vuelo, vuelo.str().c_str());

   std::stringstream puesto;
   puesto << num_puesto_checkin;
   strcpy(buffer_puesto, puesto.str().c_str());

   char *args_generador_pasajeros[] = { (char*) "generador_pasajeros", (char*) buffer_vuelo, //num_vuelo
                                        (char*) buffer_puesto, NULL };
	Process generator("generador_pasajeros", args_generador_pasajeros);
}

int main(int argc, char** argv) try {
   if (argc < 2) {
      Log::crit("Insuficientes parametros para controlador de vuelo (num_vuelo)\n");
      return (1);
   }

   int num_vuelo = atoi(argv[1]);
   ApiControladorDeVuelo api_vuelo(PATH_KEYS, num_vuelo);
   ApiTorreDeControl api_torre( std::string(PATH_KEYS).append(PATH_TORRE_DE_CONTROL).c_str());

   Log::info("ControladorDeVuelos(%d) pido puesto de checkin y zona a torre de control para vuelo %d", num_vuelo, num_vuelo);

   int num_puesto_checkin = api_torre.pedir_puesto_checkin(num_vuelo);
   int zona_utilizada = api_torre.pedir_zona(num_vuelo);
   Log::info("ControladorDeVuelos(%d) respuesta torre de control: num_zona=%d num_puesto_checkin=%d", num_vuelo, zona_utilizada, num_puesto_checkin);

   int duracion_checkin = SLEEP_DURACION_CHECKIN;

   try {
      sleep(SLEEP_DESPACHO_VUELO);

      //activo robot_despacho
      registrar_zona_en_uso(num_vuelo, zona_utilizada);
      Log::info("ControladorDeVuelos(%d) robot de despacho empieza a recibir equipajes para zona %d", num_vuelo, zona_utilizada);
      
      //abro checkin
      api_vuelo.iniciar_checkin(num_puesto_checkin);
      Log::info("ControladorDeVuelos(%d) incicio checkin puesto %d", num_vuelo, num_puesto_checkin);
      run_generador_pasajeros(num_vuelo, num_puesto_checkin);

      //espero cierre checkin
      sleep(duracion_checkin);
      api_vuelo.cerrar_checkin(num_puesto_checkin);
      Log::info("ControladorDeVuelos(%d) cierro checkin %d", num_vuelo, num_puesto_checkin);

      Log::info("ControladorDeVuelos(%d) termino...", num_vuelo);

   } catch (VueloNoRegistradoException) {
      Log::crit("El vuelo %d no se encuentra registrado en la BD\n", num_vuelo);
      return 1;
   }
} catch(const std::exception &e) {
   Log::crit("%s", e.what());
} catch(...) {
   Log::crit("Critical error. Unknow exception at the end of the 'main' function.");
}

int tomar_zona(int num_vuelo) {
	return num_vuelo;
}

void registrar_zona_en_uso(int num_vuelo, int num_zona) {
   ApiDespachante despachante((num_zona+1)/ZONAS_POR_ROBOT_DESPACHO, PATH_KEYS);
   despachante.asignar_vuelo(num_zona, num_vuelo);
}
