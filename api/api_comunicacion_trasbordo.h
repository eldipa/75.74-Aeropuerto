#ifndef APICOMUNICACIONTRASBORDO_H_
#define APICOMUNICACIONTRASBORDO_H_

#include "equipaje.h"
#include "cintas.h"
#include "mensajes.h"
#include "messagequeue.h"
#include "semaphoreset.h"
#include "sharedmemory.h"
#include "api_constants.h"

class ApiComunicacionTrasbordo {

private:
	SemaphoreSet * semaforos;
	SharedMemory * memoria_zonas;
	MessageQueue * cola_asignaciones;
	static const int cant_ipc = 3;
	MENSAJE_ZONA_ASIGNADA mensaje;

	int * vuelos_esperando; //MAX 30 vuelos esperando
	int * zonas_asignadas; // MAX_ZONAS

	int zona_asignada;
	int vuelo_destino;

public:
	//VueloTrasbordo proximo_vuelo_trasbordo();
	void notificar_asignacion_de_zona(int numero_de_zona, int numero_de_vuelo);
	void esperar_mensaje_zona_asignada();

	int get_zona_asignada();
	int get_vuelo_destino();

	ApiComunicacionTrasbordo(const char * directorio_de_trabajo);
	virtual ~ApiComunicacionTrasbordo();

};

#endif /* APICOMUNICACIONTRASBORDO_H_ */
