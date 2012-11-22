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

	int id_robot;
	const char* path_to_trasbordo;
	CintaCentral cinta;

public:
	//VueloTrasbordo proximo_vuelo_trasbordo();
	void poner_en_cinta_principal(const Equipaje& equipaje);
	int esperar_zona_asignada(int numero_vuelo);

	ApiTrasbordo(int id_robot, const char* path_to_trasbordo, const char* path_to_cinta_central);
	virtual ~ApiTrasbordo();

};

#endif
