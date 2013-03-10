#include <iostream>
#include <stdlib.h>
#include <unistd.h>

#include "constants.h"
#include "log.h"
#include "equipaje.h"
#include "api_control_equipajes.h"

#include <cstdio>

#if  DEBUG_ROBOT_CONTROL_COLOCA_CINTA_PRINCIPAL == 0

int main(int argc, char* argv [])
try
{

	if (argc < 3) {
		Log::crit(
			"Insuf parametros para controlador_equipajes_sospechosos,se esperaba (directorio_de_trabajo, config_file)\n");
		exit(1);
	}

	// soy productor
	ApiControlEquipajes api_control_equipajes(argv [1], argv [2], -1, MAX_SCANNERS + 1,true);//atoi(argv[3]), atoi(argv[4]));

	Log::info("Iniciando ControlEquipajeSospechoso (pos=%s)\n", argv [3]);

	for (;;) {
		Equipaje equipaje = api_control_equipajes.get_equipaje_a_controlar();
		Log::info("Recibo equipaje %d, limpio y vuelve a cinta central\n", equipaje.getRfid().rfid);

		// por ahora, limpia el equipaje y lo vuelve a poner al comienzo de la cinta central
		equipaje.set_sospechoso(false);
		api_control_equipajes.volver_a_colocar_equipaje_en_cinta_principal(equipaje);

	}

}
catch (const std::exception &e) {
	Log::crit("%s", e.what());
}
catch (...) {
	Log::crit("Critical error. Unknow exception at the end of the 'main' function.");
}

#elif DEBUG_ROBOT_CONTROL_COLOCA_CINTA_PRINCIPAL ==1

int main(int argc, char* argv [])
try
{

	if (argc < 5) {
		Log::crit(
			"Insuf parametros para controlador_equipajes_sospechosos,se esperaba (directorio_de_trabajo, config_file)\n");
		exit(1);
	}

	// soy productor
	ApiControlEquipajes api_control_equipajes(argv [1], argv [2], -1, MAX_SCANNERS + 1,true ); //atoi(argv[3]), atoi(argv[4]));

	Log::info("Iniciando ControlEquipajeSospechoso (pos=%s)\n", argv [3]);

	for (int i = 0 ; i < 3 ; i++) {
		Equipaje equipaje = api_control_equipajes.get_equipaje_a_controlar();
		printf("Recibo equipaje %s, limpio y vuelve a cinta central\n", equipaje.toString().c_str());

		// por ahora, limpia el equipaje y lo vuelve a poner al comienzo de la cinta central
		equipaje.set_sospechoso(false);
		equipaje.set_descripcion("No Sospechoso");
		api_control_equipajes.volver_a_colocar_equipaje_en_cinta_principal(equipaje);

	}

}
catch (const std::exception &e) {
	Log::crit("%s", e.what());
}
catch (...) {
	Log::crit("Critical error. Unknow exception at the end of the 'main' function.");
}

#endif
