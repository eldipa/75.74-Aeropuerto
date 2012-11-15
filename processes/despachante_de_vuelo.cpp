#include <stdlib.h>

#include "log.h"
#include "api_checkin.h"
//#include "api_despacho_de_vuelo.h"

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
		Log::crit("Insuficientes parametros para despachante de vuelo (num_vuelo)\n");
		return (1);
	}

	int num_vuelo = atoi(argv[1]);
	int zona_utilizada, duracion_checkin;
	//ApiDespachoVuelo api_vuelo(argv[2], num_vuelo);

	try {
		sleep(SLEEP_DESPACHO_VUELO);
		Log::info("DespachanteVuelos(%d) Iniciando despachante de vuelos...", num_vuelo);

		//activo robot_despacho
		zona_utilizada = tomar_zona(num_vuelo);
		Log::info(
				"DespachanteVuelos(%d) robot de despacho empieza a recibir equipajes para zona %d",
				num_vuelo, zona_utilizada);

		//activo robot_carga
		Log::info("DespachanteVuelos(%d) activo robot_carga (TODO!!!) de zona%d", num_vuelo,
				zona_utilizada);

		//abro checkin
		ApiCheckIn api_checkin(1, PATH_KEYS, 1);
		api_checkin.iniciar_checkin(num_vuelo);
		Log::info("DespachanteVuelos(%d) incicio checkin puesto %d", num_vuelo, 1);
		run_generador_pasajeros();

		//espero cierre checkin
		duracion_checkin = get_duracion_checkin(num_vuelo);
		sleep(duracion_checkin);
		api_checkin.cerrar_checkin();
		Log::info("DespachanteVuelos(%d) cierro checkin %d", num_vuelo);

		//espero llegada vuelos intercargo
		//espero fin de carga de equipajes
		//espero llegada avion
		Log::info(
				"DespachanteVuelos(%d) esperando intercargo, carga de equipajes y llegada del avion...");

		/*
		 bool fin_intercargo, fin_carga, llego_avion;
		 fin_intercargo = fin_carga = llego_avion = false;

		 while ( (!fin_intercargo) || (!fin_carga) || (!llego_avion) ) {
		 tEventoVuelo ev = api_vuelo.recibir_evento();

		 switch( ev ) {
		 case FIN_INTERCARGO:
		 Log::info("DespachanteVuelos(%d) llegaron todos los vuelos de intercargo!!!", num_vuelo);
		 fin_intercargo = true;
		 break;
		 case FIN_CARGA:
		 Log::info("DespachanteVuelos(%d) termino la carga del equipaje en el avion!!!", num_vuelo);
		 fin_carga = true;
		 break;
		 case LLEGO_AVION:
		 Log::info("DespachanteVuelos(%d) llego el avion!!!", num_vuelo);
		 llego_avion = true;
		 break;
		 };
		 }
		 */
		//Log::info("DespachanteVuelos(%d) listo para salir, libero zonas y sale el avion (TODO!!!", num_vuelo);
		// liberar zona (acceso a la BD)
		// aca le tenemos que avisar al robot_despacho/robot_carga que todos los equipajes estan en camino.x
	} catch (VueloNoRegistradoException &) {
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
