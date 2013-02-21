#include <iostream>
#include <stdlib.h>
#include <unistd.h>

#include "constants.h"
#include "log.h"
#include "equipaje.h"
#include "api_control_equipajes.h"

#include "process.h"

// static char id_consumidor[10];
// static char id_productor[10];

int main(int argc, char** argv)
try {

	if (argc < 5) {
		Log::crit(
				"Insuf parametros para robot de sospechosos,se esperaba (directorio_de_trabajo, config_file, pos_consumidor_cinta_central, pos_productor_cinta_central)\n");
		exit(1);
	}

	ApiControlEquipajes api_control_equipajes(argv[1], argv[2], atoi(argv[3]), atoi(argv[4]));

	// strcpy(id_consumidor, argv[3]);
	// strcpy(id_productor, argv[4]);


   static char* args_controlador_equipajes[] = { (char*) "control_equipaje_sospechoso", argv[1], argv[2], argv[3], argv[4], NULL };
	Process control_equipajes("control_equipaje_sospechoso", args_controlador_equipajes);

	Log::info("Iniciando robot control de equipaje sospechoso(pos=%s), cinta_central:%s\n", argv[1],
			argv[4]);

	for (;;) {
		sleep(rand() % SLEEP_ROBOT_SOSPECHOSOS);

		Log::info("esperando un equipaje sospechoso de cinta central(%s)\n", argv[4]);
		Equipaje equipaje = api_control_equipajes.obtener_proximo_equipaje_sospechoso();

		Log::info("recibo equipaje %s y lo envío a control\n",
				equipaje.toString().c_str());

		api_control_equipajes.enviar_equipaje_a_control(equipaje);
	}

}
catch (const std::exception &e) {
	Log::crit("%s", e.what());
}
catch (...) {
	Log::crit("Critical error. Unknow exception at the end of the 'main' function.");
}
