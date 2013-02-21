#include "api_comunicacion_trasbordo.h"
#include <string>
using namespace std;

#include "api_configuracion.h"

ApiComunicacionTrasbordo::ApiComunicacionTrasbordo(const char * directorio_de_trabajo, const char* config_file) :
   queue_manager( ApiConfiguracion::get_queue_manager(directorio_de_trabajo, config_file) ),
	cola_asignaciones(queue_manager->get_queue(PATH_COLA_ESCUCHA_ZONA_ASIGNADA, 0) ) {

	semaforos = new SemaphoreSet(string(directorio_de_trabajo).append(PATH_IPC_ROBOTS_INTERCARGO).c_str(), 0, 0, 0);
	memoria_zonas = new SharedMemory(string(directorio_de_trabajo).append(PATH_IPC_ROBOTS_INTERCARGO).c_str(), 1, 0, 0, false, false);

	zonas_asignadas = static_cast<int *>(memoria_zonas->memory_pointer());
	vuelos_esperando = zonas_asignadas + MAX_ZONAS;

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
