#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <map>

#include "api_comunicacion_intercargo.h"

#include "contenedor.h"
#include "constants.h"
#include "log.h"

#include "process.h"

static char id_vuelo_destino[10];
static char directorio_de_trabajo[MAX_PATH_SIZE];

static char *args_robot_intercargo[] = { (char*) "robot_intercargo",
		directorio_de_trabajo, (char*) id_vuelo_destino, NULL };

void lanzar_cargador(int vuelo_destino) {
	sprintf(id_vuelo_destino, "%d", vuelo_destino);
	Process cargador("robot_intercargo", args_robot_intercargo);
}

int main(int argc, char* argv[]) try{
	char path_archivo_vuelos_entrantes[MAX_PATH_SIZE];
	char path_archivo_vuelos_interconexion[MAX_PATH_SIZE];
	char primera_linea[300];
	FILE * vuelos_entrantes;
	FILE * archivo_vuelos_interconexion;
	int sleep_time;
	int numero_vuelo_entrante;
	int numero_vuelo_destino;

	ApiComunicacionIntercargo api_comunicacion(argv[1]);
	strncpy(directorio_de_trabajo, argv[1], MAX_PATH_SIZE);

	if (argc < 4) {
		Log::crit(
				"Insuficientes parametros para robot de Intercargo, se esperaba (directorio_de_trabajo, path_archivo_vuelos_entrantes, path_archivo_vuelos_interconexion)\n");
		exit(1);
	}

	std::map<int, int> vuelos_lanzados;
	std::multimap<int, int> vuelos_interconexion;
	std::multimap<int, int>::iterator it;

	strncpy(path_archivo_vuelos_entrantes, argv[2], MAX_PATH_SIZE);
	strncpy(path_archivo_vuelos_interconexion, argv[3], MAX_PATH_SIZE);

	archivo_vuelos_interconexion = fopen(path_archivo_vuelos_interconexion, "rt");
	fscanf(archivo_vuelos_interconexion, "%[^\n]\n", primera_linea);
	while (fscanf(archivo_vuelos_interconexion, "%d:%d\n", &numero_vuelo_entrante,
			&numero_vuelo_destino) != EOF) {
		vuelos_interconexion.insert(
				std::pair<int, int>(numero_vuelo_entrante, numero_vuelo_destino));
	}
	fclose(archivo_vuelos_interconexion);
	Log::info("Equipajes Leidos");

	vuelos_entrantes = fopen(path_archivo_vuelos_entrantes, "rt");
	fscanf(vuelos_entrantes, "%[^\n]\n", primera_linea);

	while (fscanf(vuelos_entrantes, "%d:%d\n", &sleep_time, &numero_vuelo_entrante) != EOF) {
		sleep(sleep_time);
		Log::info("Arribo de vuelo %d", numero_vuelo_entrante);
		for (it = vuelos_interconexion.equal_range(numero_vuelo_entrante).first;
				it != vuelos_interconexion.equal_range(numero_vuelo_entrante).second; ++it) {
			numero_vuelo_destino = (*it).second;
			if (vuelos_lanzados.count(numero_vuelo_destino) == 0) {
				lanzar_cargador(numero_vuelo_destino);
				vuelos_lanzados.insert(std::pair<int, int>(numero_vuelo_destino, 1));
			}
			api_comunicacion.informar_vuelo_entrante(numero_vuelo_entrante, numero_vuelo_destino);
		}
		vuelos_interconexion.erase(numero_vuelo_entrante);
	}
	fclose(vuelos_entrantes);
}
catch (const std::exception &e) {
	Log::crit("%s", e.what());
}
catch (...) {
	Log::crit("Critical error. Unknow exception at the end of the 'main' function.");
}

