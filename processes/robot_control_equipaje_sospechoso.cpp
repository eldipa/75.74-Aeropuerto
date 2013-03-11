#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <cstdio>

#include "constants.h"
#include "log.h"
#include "equipaje.h"
#include "api_control_equipajes.h"

#include "process.h"

#if  DEBUG_ROBOT_CONTROL_EXTRAE_CINTA_PRINCIPAL == 0

static char working_dir_control_equipaje_sospechoso [MAX_PATH_SIZE] = "/tmp/wd_control_equipaje_sospechoso";

int main(int argc, char** argv)
try
{

	if (argc < 5) {
		Log::crit(
			"Insuf parametros para robot de sospechosos,se esperaba (directorio_de_trabajo, config_file, pos_consumidor_cinta_central, pos_productor_cinta_central)\n");
		exit(1);
	}

	// soy consumidor de la cinta (el ultimo)
	ApiControlEquipajes api_control_equipajes(argv [1], argv [2], CANTIDAD_MAX_CONSUMIDORES_CINTA_CENTRAL, -1, true);

	// strcpy(id_consumidor, argv[3]);
	// strcpy(id_productor, argv[4]);

	static char* args_controlador_equipajes [] = {
		(char*)"control_equipaje_sospechoso", working_dir_control_equipaje_sospechoso, argv [2], NULL};
	Process control_equipajes("control_equipaje_sospechoso", args_controlador_equipajes);

	Log::info("Iniciando robot control de equipaje sospechoso(pos=%s), cinta_central:%s\n", argv [1], argv [4]);

	for (; ;) {
		sleep(rand() % SLEEP_ROBOT_SOSPECHOSOS);

		Log::info("Tomando Equipaje Sospechoso de Cinta Principal\n");
		Equipaje equipaje = api_control_equipajes.obtener_proximo_equipaje_sospechoso();
		Log::info("Equipaje Sospechoso (%s) extraido de Cinta Principal\n", equipaje.toString().c_str());

		if (equipaje.getRfid().rfid != 0) {
			Log::info("Enviando Equipaje Sospechoso (%s) a Control Equipajes\n", equipaje.toString().c_str());
			api_control_equipajes.enviar_equipaje_a_control(equipaje);
			Log::info("Equipaje Sospechoso (%s) enviado a Control Equipajes\n", equipaje.toString().c_str());
		}
	}

}
catch (const std::exception &e) {
	Log::crit("%s", e.what());
}
catch (...) {
	Log::crit("Critical error. Unknow exception at the end of the 'main' function.");
}

#elif  DEBUG_ROBOT_CONTROL_EXTRAE_CINTA_PRINCIPAL == 1

int main(int argc, char** argv)
try
{
	printf("Debuggeando robot_control_equipaje\n");
	if (argc < 3) {
		Log::crit(
			"Insuf parametros para robot de sospechosos,se esperaba (directorio_de_trabajo, config_file)\n");
		exit(1);
	}

	// soy consumidor de la cinta (el ultimo)
	ApiControlEquipajes api_control_equipajes(argv [1], argv [2], CANTIDAD_MAX_CONSUMIDORES_CINTA_CENTRAL, -1, true);

	for (int i = 0; i < 3; i++) {

		printf("Esperando un equipaje sospechoso de cinta central(%d)\n", CANTIDAD_MAX_CONSUMIDORES_CINTA_CENTRAL);

		Equipaje equipaje = api_control_equipajes.obtener_proximo_equipaje_sospechoso();

		printf("Recibo equipaje %s y lo envío a control\n", equipaje.toString().c_str());

		api_control_equipajes.enviar_equipaje_a_control(equipaje);
	}

}
catch (const std::exception &e) {
	Log::crit("%s", e.what());
}
catch (...) {
	Log::crit("Critical error. Unknow exception at the end of the 'main' function.");
}

#endif
