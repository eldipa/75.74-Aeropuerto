#include <iostream>
#include <stdlib.h>
#include <unistd.h>

#include "api_zona.h"
#include "api_admin.h"
#include "api_control_equipajes.h"
#include "log.h"

#include "contenedor.h"
#include "equipaje.h"

#include <map>
#include <string>
#include "cintas.h"

/*
 * Argumentos para el robot de control de equipajes.
 * ./robot_control_equipaje_sospechoso <id_robot_productor> <id_robot_consumidor> <cinta_principal>
 **/

#include <set>

int main(int argc, char** argv) {

	if (argc < 4) {
		Log::crit(
				"Insuficientes parametros para robot de despacho, se esperaba (id_productor_cinta_ppal, id_ult_consumidor_cinta_ppal, path_cinta_central)\n");
		exit(1);
	}

	ApiAdmin admin;
	bool realmente_es_sospechoso;
	ApiControlEquipajes control_equipajes(atoi(argv[1]), atoi(argv[2]), argv[3]);

	Log::info("Iniciando robot control de equipaje sospechoso(%s,%s), cinta_central:%s\n", argv[1],
			argv[2], argv[3]);

	for (;;) {
		sleep(rand() % 10);


		Log::info("Robot control equipaje(%s) esperando un equipaje sospechoso de cinta(%s)\n",
				argv[1], argv[2]);
		Equipaje equipaje = control_equipajes.obtener_proximo_equipaje_sospechoso();
		int num_vuelo = admin.get_numero_vuelo(equipaje);
		int num_zona = admin.get_numero_zona(num_vuelo);

		// ES SOSPECHOSO???
		// ESTO ES PARA PRUEBA, ALTERNA EQUIPAJES SOSPECHOSOS
		realmente_es_sospechoso = !realmente_es_sospechoso;

		if (realmente_es_sospechoso) {
			// Tendria que avisar a la torre de control, por ahora dummy
			Log::info(
					"Robot control equipaje(%s), toma el equipaje %s con destino al vuelo %d (zona %d): Es Sospechoso!",
					argv[1], equipaje.toString().c_str(), num_vuelo, num_zona);
			control_equipajes.avisar_equipaje_sospechoso(equipaje.getRfid());
		} else {
			Log::info(
					"Robot control equipaje(%s), toma el equipaje %s con destino al vuelo %d (zona %d): No es Sospechoso, volviendo a colocarlo en cinta principal",
					argv[1], equipaje.toString().c_str(), num_vuelo, num_zona);
			equipaje.setSospechoso(false);
			control_equipajes.volver_a_colocar_equipaje_en_cinta_principal(equipaje);
		}

	}

}
