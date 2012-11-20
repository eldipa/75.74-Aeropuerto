#include <iostream>
#include <stdlib.h>
#include <unistd.h>

#include "api_trasbordo.h"

#include "contenedor.h"
#include "constants.h"
#include "log.h"

std::vector<Equipaje> equipajes_para(VueloTrasbordo);

int main(int argc, char** argv) try {
	char path_torre_control[300];
	char path_cinta_central[300];
	if (argc < 2) {
		Log::crit(
				"Insuficientes parametros para robot de Intercargo, se esperaba (id)\n");
		exit(1);
	}

	strcpy(path_torre_control, PATH_KEYS);
	strcat(path_torre_control, PATH_TORRE_DE_CONTROL);
	strcpy(path_cinta_central, PATH_KEYS);
	strcat(path_cinta_central, PATH_CINTA_CENTRAL);

	ApiTrasbordo api_trasbordo(atoi(argv[1]), path_torre_control, path_cinta_central);

	for (;;) {
		Log::info("Esperando que las equipajes listos para el trasbordo");
		VueloTrasbordo trasbordo = api_trasbordo.proximo_vuelo_trasbordo();

		Log::info("Nuevo vuelo de trasbordo %i -> %i", trasbordo.vuelo_origen,
				trasbordo.vuelo_destino);

		sleep(rand() % SLEEP_TRASBORDO);

		Log::info("Sacando equipajes %i -> %i", trasbordo.vuelo_origen, trasbordo.vuelo_destino);

		std::vector<Equipaje> equipajes = equipajes_para(trasbordo);

		for (std::vector<Equipaje>::iterator equipaje = equipajes.begin();
				equipaje < equipajes.end(); equipaje++) {
			sleep(rand() % SLEEP_TRASBORDO);

			Log::info("Sacando equipaje %i", (int) *equipaje);

			sleep(rand() % SLEEP_TRASBORDO);

			Log::info("Poniendo equipaje %i en la cinta central", (int) *equipaje);

			api_trasbordo.poner_en_cinta_principal(*equipaje);
		}

		Log::info("Terminé con los equipajes %i -> %i", trasbordo.vuelo_origen,
				trasbordo.vuelo_destino);

	}
// avisar_contenedor_disponible ????

} catch(const std::exception &e) {
   Log::crit("%s", e.what());
} catch(...) {
   Log::crit("Critical error. Unknow exception at the end of the 'main' function.");
}

std::vector<Equipaje> equipajes_para(VueloTrasbordo trasbordo) {
	std::vector<Equipaje> equipajes;

	int rfid_inicial = trasbordo.vuelo_origen + trasbordo.vuelo_destino;

	// Por ahora siempre crear 3 equipajes para cada vuelo trasbordo
	for (int i = rfid_inicial; i < rfid_inicial + 3; i++) {
		Equipaje equipaje(Rfid(i, trasbordo.vuelo_destino));
		equipajes.push_back(equipaje);
	}

	return equipajes;
}
