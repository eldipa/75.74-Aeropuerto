#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <unistd.h>
#include "log.h"

#include "api_avion.h"

int main(int argc, char** argv)
try {
	//bool vuelo_cargado;
	std::vector<Contenedor> contenedores;

	if (argc < 2) {
		Log::crit(
				"Insuficientes parametros para avion, se esperaba (directorio_de_trabajo, numero_de_vuelo)\n");
		exit(1);
	}

	ApiAvion vuelo(argv[1], atoi(argv[2]));
	//vuelo_cargado = false;
	// Acá tendría que ir la lógica de la torre de control
	// Por ahora harcodeo una notificación de llegada de vuelo
	//while (!vuelo_cargado) {
	// carga de pasajeros
	sleep(rand() % 10);
	vuelo.notificar_avion_en_zona();
	contenedores = vuelo.esperar_carga_equipajes();
	vuelo.notificar_depegue();
	//}

	Log::info("Nada mas que hacer");
	return 0;
}
catch (const std::exception &e) {
	Log::crit("%s", e.what());
}
catch (...) {
	Log::crit("Critical error. Unknow exception at the end of the 'main' function.");
}
