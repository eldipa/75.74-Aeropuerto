#ifndef APITRASBORDO_H_
#define APITRASBORDO_H_

#include "equipaje.h"
#include "cintas.h"
#include "mensajes.h"
#include "messagequeue.h"
#include "semaphoreset.h"
#include "sharedmemory.h"

class ApiTrasbordo {

private:
	SemaphoreSet * semaforos;
	SharedMemory * memoria_zonas;
	MessageQueue * cola_cargadores_equipaje;
	CintaCentral * cinta;

	int id_productor;
	const char* path_to_trasbordo;
	MENSAJE_VUELO_ENTRANTE mensaje;
	static const int cant_ipc = 3;

	int * vuelos_esperando; //MAX 30 vuelos esperando
	int * zonas_asignadas; // MAX_ZONAS

public:

	void poner_en_cinta_principal(const Equipaje& equipaje);
	int esperar_zona_asignada(int numero_vuelo);
	int esperar_vuelo_entrante(int numero_vuelo_destino);

	ApiTrasbordo(const char* path_to_trasbordo, char * path_to_cinta_central, int id_instancia);
	virtual ~ApiTrasbordo();

};

#endif
