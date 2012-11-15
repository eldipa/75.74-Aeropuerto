#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "api_scanner.h"
#include "log.h"
#include "cintas.h"
#include "constants.h"

int main(int argc, char *argv[]) {
	int id_robot;
	int id_cinta_scanner;
	char path_cinta_scanner[300];
	char path_cinta_central[300];
	if (argc < 4) {
		Log::crit("Insuficientes parametros para scanner, se esperaba (id, id_cinta_scanner)\n");
		return (1);
	}

	id_robot = atoi(argv[1]); // de 1 a N
	id_cinta_scanner = atoi(argv[2]);

	strcpy(path_cinta_scanner, PATH_KEYS);
	strcat(path_cinta_scanner, PATH_CINTA_SCANNER);
	strcpy(path_cinta_central, PATH_KEYS);
	strcat(path_cinta_central, PATH_CINTA_CENTRAL);

	CintaScanner cinta_scanner(path_cinta_scanner, id_cinta_scanner);
	ApiScanner api_escaner_cinta_central(id_robot, path_cinta_central);

	Log::info("Iniciando scanner(%s), %s\n", argv[1], argv[2]);

	for (;;) {
		Log::info("Scanner(%s) Intentando tomar un nuevo equipaje de cinta de scanner(%s)\n",
				argv[1], argv[3]);
		Equipaje equipaje = cinta_scanner.sacar_equipaje(id_robot);

		Log::info("Scanner(%s) Escaneando equipaje %s\n", argv[1], equipaje.toString().c_str());
		equipaje.set_sospechoso((rand() % 5) == 0);
		sleep(rand() % SLEEP_ROBOT_SCANNER);

		if (equipaje.es_sospechoso()) {
			Log::info("Scanner(%s) se encontro sospechoso el equipaje %s\n", argv[1],
					equipaje.toString().c_str());
		} else {
			Log::info("Scanner(%s) equipaje limpio: %s\n", argv[1], equipaje.toString().c_str());
		}

		Log::info("Scanner(%s) pasando equipaje a cinta central (%s)\n", argv[1], argv[2]);
		api_escaner_cinta_central.colocar_equipaje_en_cinta_principal(equipaje);
	}

}

