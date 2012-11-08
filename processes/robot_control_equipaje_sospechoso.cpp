#include <iostream>
#include <stdlib.h>
#include <unistd.h>

#include "constants.h"
#include "log.h"
#include "equipaje.h"
#include "api_control_equipajes.h"

int main(int argc, char** argv) {

	if (argc < 4) {
		Log::crit(
				"Insuf parametros para robot de sospechosos,se esperaba (pos_cinta_central, path_cinta_central, id_cinta_central\n");
		exit(1);
	}

	ApiControlEquipajes control_equipajes(atoi(argv[1]), argv[2], atoi(argv[3]));

	Log::info("Iniciando robot control de equipaje sospechoso(pos=%s), cinta_central:%s\n", argv[1], argv[3]);

	for (;;) {
		sleep(rand() % SLEEP_ROBOT_SOSPECHOSOS);

		Log::info("Robot control equipaje(pos=%s) esperando un equipaje sospechoso de cinta central(%s)\n",argv[1], argv[3]);
		Equipaje equipaje = control_equipajes.obtener_proximo_equipaje_sospechoso();
      
		Log::info("Robot control equipaje(pos=%s) recibo equip %s, limpio y vuelve a cinta central\n",argv[1], equipaje.toString().c_str());
      // por ahora, limpia el equipaje y lo vuelve a poner al comienzo de la cinta central
      equipaje.set_sospechoso(false);
      control_equipajes.volver_a_colocar_equipaje_en_cinta_principal(equipaje);
	}

}
