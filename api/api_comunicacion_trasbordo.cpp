#include "api_comunicacion_trasbordo.h"

ApiComunicacionTrasbordo::ApiComunicacionTrasbordo(char * path_to_robots_intercargo,
		char * path_cola_comunicacion_trasbordo, int id_instancia) {

	semaforos = new SemaphoreSet(path_to_robots_intercargo, cant_ipc * id_instancia, 0, 0);
	memoria_zonas = new SharedMemory(path_to_robots_intercargo, cant_ipc * id_instancia + 1, 0, 0,
			false, false);
	cola_asignaciones = new MessageQueue(path_cola_comunicacion_trasbordo, cant_ipc * id_instancia + 2);

}

ApiComunicacionTrasbordo::~ApiComunicacionTrasbordo() {
	// TODO Auto-generated destructor stub
}

void ApiComunicacionTrasbordo::notificar_asignacion_de_zona(int numero_de_zona,
		int numero_de_vuelo) {
	int i;

	semaforos->wait_on(0);

	zonas_asignadas[numero_de_zona - 1] = numero_de_vuelo;

	for (i = 0; i < MAX_ROBOTS_INTERCARGO_ESPERANDO_POR_ZONAS; i++) {
		if (vuelos_esperando[i] == numero_de_vuelo) {
			vuelos_esperando[i] = 0;
			semaforos->signalize(i + 1);
		}
	}

	semaforos->signalize(0);
}

void ApiComunicacionTrasbordo::esperar_mensaje_zona_asignada() {
	cola_asignaciones->pull(&mensaje, sizeof(MENSAJE_ZONA_ASIGNADA) - sizeof(long), 0);
	zona_asignada = mensaje.zona_asignada;
	vuelo_destino = mensaje.numero_de_vuelo;
}

int ApiComunicacionTrasbordo::get_zona_asignada() {
	return zona_asignada;
}

int ApiComunicacionTrasbordo::get_vuelo_destino() {
	return vuelo_destino;
}
