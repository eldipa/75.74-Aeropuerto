#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "api_scanner.h"
#include "log.h"
#include "cintas.h"
#include "constants.h"

int main(int argc, char *argv[]) {
	if (argc < 4) {
		Log::crit(
				"Insuficientes parametros para scanner, se esperaba (id, path_cinta_scanner, id_cinta_scanner path_cinta_central id_cinta_central)\n");
		return (1);
	}

	CintaScanner cinta_scanner(argv[2], atoi(argv[3]));
	ApiScanner api_escaner_cinta_central( atoi(argv[1]), argv[4], atoi(argv[5]));


	Log::info("Iniciando scanner(%s), %s, %s\n", argv[1], argv[3], argv[5]);

	for (;;) {
		Log::info("Scanner(%s) Intentando tomar un nuevo equipaje de cinta de scanner(%s)\n", argv[1], argv[3]);
		Equipaje equipaje = cinta_scanner.sacar_equipaje();

		Log::info("Scanner(%s) Escaneando equipaje %s\n", argv[1], equipaje.toString().c_str());
		//equipaje.set_sospechoso( (rand() % 100) == 0 );
      equipaje.set_sospechoso(true);
		sleep(rand() % SLEEP_ROBOT_SCANNER);

		if (equipaje.es_sospechoso()) {
			Log::info("Scanner(%s) se encontro sospechoso el equipaje %s\n", argv[1], equipaje.toString().c_str());
		} else {
			Log::info("Scanner(%s) equipaje limpio: %s\n", argv[1], equipaje.toString().c_str());
		}

		Log::info("Scanner(%s) pasando equipaje a cinta central (%s)\n", argv[1], argv[3]);
		api_escaner_cinta_central.colocar_equipaje_en_cinta_principal(equipaje);
	}

}

