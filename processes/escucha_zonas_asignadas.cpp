#include <cstring>
#include <cstdlib>
#include "constants.h"
#include "log.h"
#include <iostream>

#include "api_comunicacion_trasbordo.h"

void print_args(int argc, char * argv []){
	int i;
	std::cout << "argc=" << argc << std::endl;
	std::cout << argv[0];
	for(i=1;i<argc;i++){
		std::cout << " " << argv[i];
	}
	std::cout << std::endl;
}

int main(int argc, char* argv[])
try {
	//chdir("processes");


	if (argc < 3) {
		Log::crit(
				"Insuficientes parametros para Escucha Zonas Asignadas, se esperaba (directorio_de_trabajo, config_file)\n");
		exit(1);
	}
	print_args(argc,argv);

	ApiComunicacionTrasbordo trasbordo(argv[1], argv[2]);

	for (;;) {
		Log::info("Esperando asignacion de zona");
		trasbordo.esperar_mensaje_zona_asignada();
		Log::info("Zona %d asignada al vuelo %d", trasbordo.get_zona_asignada(),
				trasbordo.get_vuelo_destino());
		trasbordo.notificar_asignacion_de_zona(trasbordo.get_zona_asignada(),
				trasbordo.get_vuelo_destino());
	}
}
catch (const std::exception &e) {
	Log::crit("%s", e.what());
}
catch (...) {
	Log::crit("Critical error. Unknow exception at the end of the 'main' function.");
}
