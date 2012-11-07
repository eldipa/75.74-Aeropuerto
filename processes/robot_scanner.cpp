#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "api_scanner.h"
#include "log.h"
#include "cintas.h"

int main(int argc, char *argv[]) {
	unsigned int id;

	if (argc < 4) {
		Log::crit(
				"Insuficientes parametros para scanner, se esperaba (id, path_cinta_scanner, id_cinta_scanner path_cinta_central id_cinta_central)\n");
		return (1);
	}
	sscanf(argv[1], "%ud", &id);

	ApiScanner api_escaner_cinta_central(id, argv[4]);
	CintaScanner cinta_scanner(argv[2], atoi(argv[3]));
	//CintaCentral cinta_central(argv[4], atoi(argv[5]));

	Log::info("Iniciando scanner(%s), %s, %s\n", argv[1], argv[2], argv[3]);

	for (;;) {
		Log::info("Scanner(%s) Intentando tomar un nuevo equipaje de cinta(%s)\n", argv[1],
				argv[2]);
		Equipaje equipaje = cinta_scanner.sacar_equipaje();
		Log::info("Scanner(%s) Escaneando equipaje %s\n", argv[1], equipaje.toString().c_str());

		bool sospechoso = (rand() % 100) == 0;
		sleep(rand() % 10);

		if (sospechoso) {
			Log::info("Scanner(%s) se encontro sospechoso el equipaje %s\n", argv[1],
					equipaje.toString().c_str());
			// equipaje_sospechoso no seria parte de la api del escaner
			// solamente se marca el rfid como sospechoso
			// en el despachante tiene que haber algo del estilo equipaje sospechoso
			// total el despachante es el que tiene que dejarlo pasar
			//api.equipaje_sospechoso(equipaje);
		} else {
			Log::info("Scanner(%s) equipaje limpio: %s\n", argv[1], equipaje.toString().c_str());
			//api.equipaje_limpio(equipaje);
		}

		Log::info("Scanner(%s) pasando equipaje a cinta central (%s)\n", argv[1], argv[3]);
		api_escaner_cinta_central.colocar_equipaje_en_cinta_principal(equipaje);
	}

}
