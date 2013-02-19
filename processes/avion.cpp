#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <unistd.h>
#include "log.h"

#include "api_avion.h"

int get_total_valijas(const std::vector<Contenedor>&);

int main(int argc, char** argv)
try {
	//bool vuelo_cargado;
	std::vector<Contenedor> contenedores;

	if (argc < 3) {
		Log::crit(
				"Insuficientes parametros para avion, se esperaba (directorio_de_trabajo, config_file, numero_de_vuelo)\n");
		exit(1);
	}

	ApiAvion vuelo(argv[1], argv[2], atoi(argv[3]));
	//vuelo_cargado = false;
	// Acá tendría que ir la lógica de la torre de control
	// Por ahora harcodeo una notificación de llegada de vuelo
	//while (!vuelo_cargado) {
	// carga de pasajeros
	sleep(rand() % 10);
	vuelo.notificar_avion_en_zona();
	contenedores = vuelo.esperar_carga_equipajes();

   Log::info("Despegando vuelo %d con %d contenedores y %d valijas", atoi(argv[3]), contenedores.size(), get_total_valijas(contenedores));

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

int get_total_valijas(const std::vector<Contenedor>& contenedores) {
   int cant_valijas = 0;
   std::vector<Contenedor>::const_iterator it = contenedores.begin();
   for(; it != contenedores.end(); it++)
      cant_valijas += it->get_equipajes();
   return cant_valijas;
}
