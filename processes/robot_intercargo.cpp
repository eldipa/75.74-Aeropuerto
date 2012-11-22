#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <cstring>
#include <cstdio>
#include "cintas.h"

#include "api_trasbordo.h"

#include "contenedor.h"
#include "constants.h"
#include "log.h"

#include <map>

void cargar_equipajes(int numero_vuelo_destino, std::multimap<int, Equipaje> & equipajes_a_cargar) {
	char path_archivo_equipajes[300];
	FILE * equipajes_vuelo_destino;
	char descripcion_equipaje[SIZE_DESCRIPCION_EQUIPAJE];
	int peso_equipaje, rfid, vuelo_destino;
	char escala_destino[MAX_SIZE_ESCALA];
	int escalas[MAX_CANT_ESCALAS];
	char primera_linea[300];
	int numero_vuelo_entrante;

	sprintf(path_archivo_equipajes, "equipaje_destino_%d.csv", numero_vuelo_destino);

	equipajes_vuelo_destino = fopen(path_archivo_equipajes, "rt");

	fscanf(equipajes_vuelo_destino, "%s\n", primera_linea);

	while (fscanf(equipajes_vuelo_destino, "%d:%d:%s:%d:%d:%d:%d:%d:%d:%d:%s\n",
			&numero_vuelo_entrante, &peso_equipaje, descripcion_equipaje, &rfid, &vuelo_destino,
			escalas, escalas + 1, escalas + 2, escalas + 3, escalas + 4, escala_destino) != EOF) {
		Rfid rf(rfid, numero_vuelo_destino);
		strncpy(rf.escala_destino, escala_destino, MAX_SIZE_ESCALA);
		rf.sospechoso = false;
		memcpy(rf.escalas, escalas, sizeof(escalas));

		Equipaje equipaje(rf, peso_equipaje);
		equipaje.set_descripcion(descripcion_equipaje);
		equipajes_a_cargar.insert(std::pair<int, Equipaje>(numero_vuelo_entrante, equipaje));
	}

	fclose(equipajes_vuelo_destino);
}

int main(int argc, char** argv) {
	char path_ipc_trasbordo[300];
	char path_cinta_principal[300];
	int zona_asignada;
	int numero_vuelo_destino;
	int numero_vuelo_entrante;
	std::multimap<int, Equipaje>::iterator it;


	if (argc < 2) {
		Log::crit(
				"Insuficientes parametros para robot de Intercargo, se esperaba (num_vuelo_destino)\n");
		exit(1);
	}

	numero_vuelo_destino = atoi(argv[1]);

	strcpy(path_ipc_trasbordo, PATH_KEYS);
	strcat(path_ipc_trasbordo, PATH_COLA_CARGADORES_TRASBORDO);
	strcpy(path_cinta_principal, PATH_KEYS);
	strcat(path_cinta_principal, PATH_CINTA_CENTRAL);

	ApiTrasbordo api_trasbordo(path_ipc_trasbordo, path_cinta_principal, 0);

	std::multimap<int, Equipaje> equipajes_a_cargar;

	cargar_equipajes(numero_vuelo_destino, equipajes_a_cargar);

	Log::info("Esperando que se asigne la zona para vuelo %d", numero_vuelo_destino);

	zona_asignada = api_trasbordo.esperar_zona_asignada(numero_vuelo_destino);

	while (!equipajes_a_cargar.empty()) {
		Log::info("Esperando vuelos entrantes con equipaje destino a %d", numero_vuelo_destino);
		numero_vuelo_entrante = api_trasbordo.esperar_vuelo_entrante(numero_vuelo_destino);
		Log::info("Comenzando a cargar equipaje de vuelo entrante %d", numero_vuelo_entrante);
		for (it = equipajes_a_cargar.equal_range(numero_vuelo_entrante).first;
				it != equipajes_a_cargar.equal_range(numero_vuelo_entrante).second; ++it) {
			Log::info("Colocando equipaje %d:%s", (*it).second.getRfid().rfid,
					(*it).second.get_descripcion());
			api_trasbordo.poner_en_cinta_principal((*it).second);
		}
		equipajes_a_cargar.erase(numero_vuelo_entrante);
	}
	Log::info("Terminé con los equipajes con destino a %d", numero_vuelo_destino);

}

