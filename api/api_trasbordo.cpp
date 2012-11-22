#include "api_trasbordo.h"
#include "api_constants.h"
#include "messagequeue.h"
#include <exception>
#include <stdexcept>

ApiTrasbordo::ApiTrasbordo(const char* path_to_trasbordo, char * path_to_cinta_central,
		int id_instancia) :
		path_to_trasbordo(path_to_trasbordo) {

	semaforos = new SemaphoreSet(path_to_trasbordo, cant_ipc * id_instancia, 0, 0);
	memoria_zonas = new SharedMemory(path_to_trasbordo, cant_ipc * id_instancia + 1, 0, 0, false,
			false);
	cola_cargadores_equipaje = new MessageQueue(path_to_trasbordo, cant_ipc * id_instancia + 2);
	cinta = new CintaCentral(path_to_cinta_central);
	id_productor = -1;
}

ApiTrasbordo::~ApiTrasbordo() {
	delete semaforos;
	delete memoria_zonas;
	delete cola_cargadores_equipaje;
	delete cinta;
}

void ApiTrasbordo::poner_en_cinta_principal(const Equipaje& equipaje) {
	cinta->colocar_elemento(&equipaje, id_productor);
}

int ApiTrasbordo::esperar_vuelo_entrante(int numero_vuelo_destino) {
	mensaje.mtype = numero_vuelo_destino;
	cola_cargadores_equipaje->pull(&mensaje, sizeof(MENSAJE_VUELO_ENTRANTE) - sizeof(long),
			numero_vuelo_destino);
	return mensaje.vuelo_entrante;
}

int ApiTrasbordo::esperar_zona_asignada(int numero_vuelo) {
	int zona_asignada = -1;
	int i;

	while (zona_asignada == -1) {
		semaforos->wait_on(0);

		for (i = 0; i < MAX_ZONAS; i++) {
			if (zonas_asignadas[i] == numero_vuelo) {
				zona_asignada = i + 1;
				break;
			}
		}

		if (zona_asignada == -1) {
			for (i = 0; i < MAX_ROBOTS_INTERCARGO_ESPERANDO_POR_ZONAS; i++) {
				if (vuelos_esperando[i] == 0) {
					vuelos_esperando[i] = numero_vuelo;
					semaforos->signalize(0);
					semaforos->wait_on(i + 1);
					break;
				}
			}
		} else {
			semaforos->signalize(0);
		}
	}
	id_productor = zona_asignada + 5;
	return zona_asignada;
}

