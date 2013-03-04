#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <map>
#include <iostream>

#include "api_comunicacion_intercargo.h"

#include "contenedor.h"
#include "constants.h"
#include "log.h"

#include <unistd.h>
#include "process.h"
#include "dir.h"

static char id_vuelo_destino [10];
static char directorio_de_trabajo [MAX_PATH_SIZE];
static char config_file [MAX_PATH_SIZE];

char directorio_de_trabajo_relativo [FILENAME_MAX];
char config_file_relativo [FILENAME_MAX];

static char *args_controlador_intercargo [] = {
	(char*)"controlador_equipaje_intercargo", directorio_de_trabajo_relativo, config_file_relativo, (char*)id_vuelo_destino, NULL};

// static char *args_escucha_zonas [] = {
// 	(char*)"escucha_zonas_asignadas", directorio_de_trabajo_relativo, config_file_relativo, NULL};

// void lanzar_escucha_zonas() {
// 	char current_working_dir [FILENAME_MAX];
// 	char launch_dir [FILENAME_MAX];

// 	if (!getcwd(current_working_dir, sizeof(current_working_dir)))
// 		throw GenericError("Unable to get current working dir");
// 	current_working_dir [sizeof(current_working_dir) - 1] = '\0'; /* not really required */

// 	locate_dir(launch_dir, current_working_dir, (char *)"processes");
// 	relativize_dir(directorio_de_trabajo_relativo, directorio_de_trabajo, (const char *)launch_dir,
// 		current_working_dir);
// 	relativize_dir(config_file_relativo, config_file, (const char *)launch_dir, current_working_dir);

// 	if (chdir(launch_dir) != 0) {
// 		throw GenericError("Cannot change working dir to %s", launch_dir);
// 	}

// 	Process cargador("escucha_zonas_asignadas", args_escucha_zonas);

// 	if (chdir(current_working_dir) != 0) {
// 		throw GenericError("Cannot change working dir to %s", current_working_dir);
// 	}
// }

void lanzar_cargador(int vuelo_destino) {
	char current_working_dir [FILENAME_MAX];
	char launch_dir [FILENAME_MAX];

	if (!getcwd(current_working_dir, sizeof(current_working_dir)))
		throw GenericError("Unable to get current working dir");
	current_working_dir [sizeof(current_working_dir) - 1] = '\0'; /* not really required */

	locate_dir(launch_dir, current_working_dir, (char *)"processes");
	relativize_dir(directorio_de_trabajo_relativo, directorio_de_trabajo, (const char *)launch_dir,
		current_working_dir);
	relativize_dir(config_file_relativo, config_file, (const char *)launch_dir, current_working_dir);

	if (chdir(launch_dir) != 0) {
		throw GenericError("Cannot change working dir to %s", launch_dir);
	}
	sprintf(id_vuelo_destino, "%d", vuelo_destino);
	Process cargador("controlador_equipaje_intercargo", args_controlador_intercargo);
	if (chdir(current_working_dir) != 0) {
		throw GenericError("Cannot change working dir to %s", current_working_dir);
	}
}

int main(int argc, char* argv [])
try
{
	//chdir("processes");

	char path_archivo_vuelos_entrantes [MAX_PATH_SIZE];
	char path_archivo_vuelos_interconexion [MAX_PATH_SIZE];
	char primera_linea [300];
	FILE * vuelos_entrantes;
	FILE * archivo_vuelos_interconexion;
	int sleep_time;
	int numero_vuelo_entrante;
	int numero_vuelo_destino;

	if (argc < 5) {
		Log::crit(
			"Insuficientes parametros para robot de Intercargo, se esperaba (directorio_de_trabajo, config_file, path_archivo_vuelos_entrantes, path_archivo_vuelos_interconexion)\n");
		exit(1);
	}

	strncpy(directorio_de_trabajo, argv [1], MAX_PATH_SIZE);
	strncpy(config_file, argv [2], MAX_PATH_SIZE);

	ApiComunicacionIntercargo api_comunicacion(directorio_de_trabajo, config_file, true);
	// lanzar_escucha_zonas();

	std::map<int, int> vuelos_lanzados;
	std::multimap<int, int> vuelos_interconexion;
	std::multimap<int, int>::iterator it;

	strncpy(path_archivo_vuelos_entrantes, argv [3], MAX_PATH_SIZE);
	strncpy(path_archivo_vuelos_interconexion, argv [4], MAX_PATH_SIZE);

	archivo_vuelos_interconexion = fopen(path_archivo_vuelos_interconexion, "rt");
	fscanf(archivo_vuelos_interconexion, "%[^\n]\n", primera_linea);
	while (fscanf(archivo_vuelos_interconexion, "%d:%d\n", &numero_vuelo_entrante, &numero_vuelo_destino) != EOF) {
		vuelos_interconexion.insert(std::pair<int, int>(numero_vuelo_entrante, numero_vuelo_destino));
	}
	fclose(archivo_vuelos_interconexion);
	Log::info("Equipajes Leidos");

	vuelos_entrantes = fopen(path_archivo_vuelos_entrantes, "rt");
	fscanf(vuelos_entrantes, "%[^\n]\n", primera_linea);

	while (fscanf(vuelos_entrantes, "%d:%d\n", &sleep_time, &numero_vuelo_entrante) != EOF) {
		//	sleep(sleep_time);
		Log::info("Arribo de vuelo %d", numero_vuelo_entrante);
		for (it = vuelos_interconexion.equal_range(numero_vuelo_entrante).first;
			it != vuelos_interconexion.equal_range(numero_vuelo_entrante).second ; ++it)
		{
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

	pause();
}
catch (const std::exception &e) {
	Log::crit("%s", e.what());
	std::cerr << e.what() << std::endl;
}
catch (...) {
	Log::crit("Critical error. Unknow exception at the end of the 'main' function.");
}

