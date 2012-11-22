#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <cstring>

#include "api_trasbordo.h"

#include "contenedor.h"
#include "constants.h"
#include "log.h"

#include "mensajes.h"
#include "messagequeue.h"
#include <cstdio>
#include "process.h"
#include <map>

static char id_vuelo_destino[10];

static char *args_cargador_equipaje[] = { (char*) "cargador_equipaje_trasbordo",
		(char*) id_vuelo_destino, NULL };

void lanzar_cargador(int vuelo_destino) {

	sprintf(id_vuelo_destino, "%d", vuelo_destino);
	Process cargador("cargador_equipaje_trasbordo", args_cargador_equipaje);
}

int main(int argc, char** argv) {
	char path_cola_cargadores[300];
	char path_archivo_vuelos_entrantes[300];
	char path_archivo_vuelos_interconexion[300];
	char primera_linea[300];
	FILE * vuelos_entrantes;
	FILE * archivo_vuelos_interconexion;
	int sleep_time;
	int numero_vuelo_entrante;
	int numero_vuelo_destino;

	std::multimap<int, int>::iterator it;
	MENSAJE_VUELO_ENTRANTE mensaje;

	if (argc < 3) {
		Log::crit(
				"Insuficientes parametros para robot de Intercargo, se esperaba (path_archivo_vuelos_entrantes, path_archivo_vuelos_interconexion)\n");
		exit(1);
	}

	std::map<int, int> vuelos_lanzados;
	std::multimap<int, int> vuelos_interconexion;

	strcpy(path_cola_cargadores, PATH_KEYS);
	strcat(path_cola_cargadores, PATH_COLA_CARGADORES_TRASBORDO);

	MessageQueue cola_cargadores_equipaje(path_cola_cargadores, 0);

	strcpy(path_archivo_vuelos_entrantes, argv[1]);
	strcpy(path_archivo_vuelos_entrantes, argv[2]);

	archivo_vuelos_interconexion = fopen(path_archivo_vuelos_interconexion, "rt");
	fscanf(archivo_vuelos_interconexion, "%s\n", primera_linea);
	while (fscanf(archivo_vuelos_interconexion, "%d:%d\n", &numero_vuelo_entrante,
			&numero_vuelo_destino) != EOF) {
		vuelos_interconexion.insert(
				std::pair<int, int>(numero_vuelo_entrante, numero_vuelo_destino));
	}
	fclose(archivo_vuelos_interconexion);

	vuelos_entrantes = fopen(path_archivo_vuelos_entrantes, "rt");
	fscanf(vuelos_entrantes, "%s\n", primera_linea);

	while (fscanf(vuelos_entrantes, "%d\t%d\n", &sleep_time, &numero_vuelo_entrante) != EOF) {
		sleep(sleep_time);
		Log::info("Arribo de vuelo %d", mensaje.vuelo_entrante);
		for (it = vuelos_interconexion.equal_range(numero_vuelo_entrante).first;
				it != vuelos_interconexion.equal_range(numero_vuelo_entrante).second; ++it) {
			numero_vuelo_destino = (*it).second;
			if (vuelos_lanzados.count(numero_vuelo_destino) == 0) {
				lanzar_cargador(numero_vuelo_destino);
				vuelos_lanzados.insert(std::pair<int, int>(numero_vuelo_destino, 1));
			}
			mensaje.mtype = numero_vuelo_destino;
			mensaje.vuelo_entrante = numero_vuelo_entrante;
			cola_cargadores_equipaje.push(&mensaje, sizeof(MENSAJE_VUELO_ENTRANTE) - sizeof(long));
		}
		vuelos_interconexion.erase(numero_vuelo_entrante);
	}
	fclose(vuelos_entrantes);
}
