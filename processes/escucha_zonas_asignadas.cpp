#include <cstring>
#include <cstdlib>
#include "constants.h"
#include "log.h"

#include "api_comunicacion_trasbordo.h"

int main(int argc, char* argv[])
try {

	if (argc < 3) {
		Log::crit(
				"Insuficientes parametros para Escucha Zonas Asignadas, se esperaba (directorio_de_trabajo, config_file)\n");
		exit(1);
	}

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
