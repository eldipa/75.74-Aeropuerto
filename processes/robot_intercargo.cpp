#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <cstring>
#include <cstdio>
#include "cintas.h"

#include "api_trasbordo.h"
#include "api_comunicacion_aeropuerto.h"
#include "contenedor.h"
#include "constants.h"
#include "log.h"
#include "api_configuracion.h"

#if  DEBUG_ROBOT_INTERCARGO_COLOCA_CINTA_PRINCIPAL == 0

int main(int argc, char** argv)
try
{
	if (argc < 3) {
		Log::crit(
			"Insuficientes parametros para robot de Intercargo, se esperaba (directorio_de_trabajo, config_file)\n");
		exit(1);
	}

	int id_aeropuerto = ApiConfiguracion::get_id_aeropuerto(argv[2]);
	ApiTrasbordo api_trasbordo(argv [1], argv [2], id_aeropuerto);
	ApiComunicacionAeropuerto api_comm_aeropuerto(argv[1], argv[2]);

	for(;;) {
		Equipaje equipaje_trasbordo = api_comm_aeropuerto.get_proximo_equipaje_intercargo(id_aeropuerto);
		Log::info("Colocando equipaje %d:%s", equipaje_trasbordo.getRfid().rfid, equipaje_trasbordo.get_descripcion());
		api_trasbordo.poner_en_cinta_principal(equipaje_trasbordo);
	}
}

catch (const std::exception &e) {
	Log::crit("%s", e.what());
}
catch (...) {
	Log::crit("Critical error. Unknow exception at the end of the 'main' function.");
}

#elif DEBUG_ROBOT_INTERCARGO_COLOCA_CINTA_PRINCIPAL == 1

int main(int argc, char** argv)
try
{
	if (argc < 3) {
		Log::crit(
			"Insuficientes parametros para robot de Intercargo, se esperaba (directorio_de_trabajo, config_file)\n");
		exit(1);
	}

	ApiTrasbordo api_trasbordo(argv [1], argv [2], 1);

	for (int i = 0 ; i < 3 ; i++) {
		Equipaje equipaje(Rfid(i, 1000));
		equipaje.set_sospechoso(false);
		printf("Colocando equipaje %s\n", equipaje.toString().c_str());
		api_trasbordo.poner_en_cinta_principal(equipaje);
	}
}

catch (const std::exception &e) {
	Log::crit("%s", e.what());
}
catch (...) {
	Log::crit("Critical error. Unknow exception at the end of the 'main' function.");
}

#endif
