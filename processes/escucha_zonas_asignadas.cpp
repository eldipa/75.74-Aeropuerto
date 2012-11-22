#include <iostream>
#include <stdlib.h>
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
#include "api_comunicacion_trasbordo.h"

int main(/*int argc, char** argv*/) {
	char path_ipc_intercargo[300];

	strcpy(path_ipc_intercargo, PATH_KEYS);
	strcat(path_ipc_intercargo, PATH_COLA_CARGADORES_TRASBORDO);

	ApiComunicacionTrasbordo trasbordo(path_ipc_intercargo,0);

	for (;;) {
		Log::info("Esperando asignacion de zona");
		trasbordo.esperar_mensaje_zona_asignada();
		Log::info("Zona %d asignada al vuelo %d", trasbordo.get_zona_asignada(),
				trasbordo.get_vuelo_destino());
		trasbordo.notificar_asignacion_de_zona(trasbordo.get_zona_asignada(),
				trasbordo.get_vuelo_destino());
	}

}
