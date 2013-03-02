#include <stdlib.h>

#include "log.h"
#include "api_checkin.h"
#include "api_controlador_de_vuelo.h"
#include "api_torre_de_control.h"
#include "api_despachante.h"
#include "api_comunicacion_aeropuerto.h"
#include "api_configuracion.h"

#include "process.h"
#include "constants.h"

#include "database.h"
#include "stmt.h"
#include "tupleiter.h"

#include <unistd.h>
#include <sstream>

int tomar_zona(int num_zona);
//int get_duracion_checkin(int num_vuelo);
void registrar_zona_en_uso(const char *, const char*, int num_vuelo, int num_zona);

class VueloNoRegistradoException {
public:
	VueloNoRegistradoException(int num_vuelo) :
			num_vuelo(num_vuelo) {
	}
private:
	int num_vuelo;
};


void run_generador_pasajeros(char* directorio_de_trabajo, const char* config_file, int num_vuelo, int num_puesto_checkin) {
   char buffer_vuelo[6];
   char buffer_puesto[6];

   std::stringstream vuelo;
   vuelo << num_vuelo;
   strcpy(buffer_vuelo, vuelo.str().c_str());

   std::stringstream puesto;
   puesto << num_puesto_checkin;
   strcpy(buffer_puesto, puesto.str().c_str());

   char *args_generador_pasajeros[] = { (char*) "generador_pasajeros", (char*) directorio_de_trabajo, (char*)config_file,
                                        (char*) buffer_vuelo, //num_vuelo
                                        (char*) buffer_puesto, NULL };
	Process generator("generador_pasajeros", args_generador_pasajeros);
}

int main(int argc, char** argv) try {
   if (argc < 4) {
      Log::crit("Insuficientes parametros para controlador de vuelo (directorio_de_trabajo, config_file, num_vuelo)\n");
      return (1);
   }

   int num_vuelo = atoi(argv[3]);
   ApiControladorDeVuelo api_vuelo(argv[1], argv[2], num_vuelo);
   ApiTorreDeControl api_torre(argv[1], argv[2]);
   ApiComunicacionAeropuerto api_comm_aeropuerto(argv[1], argv[2]);

   Log::info("Pido puesto de checkin libre para vuelo %d", num_vuelo);
   int num_puesto_checkin = api_torre.pedir_puesto_checkin(num_vuelo);

   Log::info("Ok, puest = %d. Pido zona libre para vuelo %d", num_puesto_checkin, num_vuelo);
   int zona_utilizada = api_torre.pedir_zona(num_vuelo);

   Log::info("Respuesta torre de control: num_vuelo=%d num_zona=%d num_puesto_checkin=%d", num_vuelo, zona_utilizada, num_puesto_checkin);

   try {
      sleep(rand()%SLEEP_DESPACHO_VUELO);

      //activo robot_despacho
      registrar_zona_en_uso(argv[1], argv[2], num_vuelo, zona_utilizada);
      Log::info("Robot de despacho empieza a recibir equipajes para vuelo %d, zona %d", num_vuelo, zona_utilizada);

      //abro checkin
      api_vuelo.iniciar_checkin(num_puesto_checkin, zona_utilizada);
      api_comm_aeropuerto.zona_asignada(num_vuelo, zona_utilizada, ApiConfiguracion::get_id_aeropuerto(argv[2]));

      Log::info("Inicio checkin puesto %d vuelo %d", num_puesto_checkin, num_vuelo);
      run_generador_pasajeros(argv[1], argv[2], num_vuelo, num_puesto_checkin);

      //espero cierre checkin
      sleep(rand()%SLEEP_DURACION_CHECKIN);
      api_vuelo.cerrar_checkin(num_puesto_checkin, zona_utilizada);
      api_torre.liberar_puesto_checkin(num_puesto_checkin);
      Log::info("cierro checkin vuelo %d y libero puesto de checkin %d", num_vuelo, num_puesto_checkin);

      Log::info("termino...");

   } catch (VueloNoRegistradoException & ) {
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

void registrar_zona_en_uso(const char * directorio_de_trabajo, const char* config_file, int num_vuelo, int num_zona) {
   ApiDespachante despachante(directorio_de_trabajo, config_file,"controlador_de_vuelo", (num_zona+1)/ZONAS_POR_ROBOT_DESPACHO);
   despachante.asignar_vuelo(num_zona, num_vuelo);
}
