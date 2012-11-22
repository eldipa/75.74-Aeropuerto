#ifndef APICOMUNICACIONTRASBORDO_H_
#define APICOMUNICACIONTRASBORDO_H_

#include "equipaje.h"
#include "cintas.h"
#include "mensajes.h"
#include "messagequeue.h"
#include "semaphoreset.h"
#include "sharedmemory.h"

class ApiComunicacionTrasbordo {

private:
	SemaphoreSet * semaforos;
	SharedMemory * memoria_zonas;
	MessageQueue * cola_asignaciones;

public:
	//VueloTrasbordo proximo_vuelo_trasbordo();
	void notificar_asignacion_de_zona(int numero_de_zona, int numero_de_vuelo);
	void esperar_mensaje_zona_asignada();

	int get_zona_asignada();
	int get_vuelo_destino();

	ApiComunicacionTrasbordo(char * path_to_robots_intercargo, int id_instancia);
	virtual ~ApiComunicacionTrasbordo();

};

#endif /* APICOMUNICACIONTRASBORDO_H_ */
