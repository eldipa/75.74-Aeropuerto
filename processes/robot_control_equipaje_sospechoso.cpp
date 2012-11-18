#include <iostream>
#include <stdlib.h>
#include <unistd.h>

#include "constants.h"
#include "log.h"
#include "equipaje.h"
#include "api_control_equipajes.h"

int main(int argc, char** argv) {
	char path[300];
	int id_productor;
	int id_consumidor;
	if (argc < 2) {
		Log::crit(
				"Insuf parametros para robot de sospechosos,se esperaba (pos_consumidor_cinta_central,pos_productor_cinta_central)\n");
		exit(1);
	}
	strcpy(path, PATH_KEYS);
	strcat(path, PATH_CINTA_CENTRAL);
	id_consumidor = atoi(argv[1]);
	id_productor = atoi(argv[2]);
	ApiControlEquipajes control_equipajes(id_consumidor, id_productor, path);

	Log::info("Iniciando robot control de equipaje sospechoso(pos=%s), cinta_central:%s\n", argv[1],
			argv[3]);

	for (;;) {
		sleep(rand() % SLEEP_ROBOT_SOSPECHOSOS);

		Log::info(
				"Robot control equipaje(pos=%s) esperando un equipaje sospechoso de cinta central(%s)\n",
				argv[1], argv[3]);
		Equipaje equipaje = control_equipajes.obtener_proximo_equipaje_sospechoso();

		Log::info(
				"Robot control equipaje(pos=%s) recibo equip %s, limpio y vuelve a cinta central\n",
				argv[1], equipaje.toString().c_str());
		// por ahora, limpia el equipaje y lo vuelve a poner al comienzo de la cinta central
		equipaje.set_sospechoso(false);
		control_equipajes.volver_a_colocar_equipaje_en_cinta_principal(equipaje);
	}

}
