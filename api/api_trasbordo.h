#ifndef APITRASBORDO_H_
#define APITRASBORDO_H_

#include "equipaje.h"
#include "cintas.h"
#include "mensajes.h"
#include "semaphoreset.h"
#include "sharedmemory.h"

#include "yasper.h"
#include "imessagequeue.h"
#include "iqueue_manager.h"

class ApiTrasbordo {

private:
   yasper::ptr<IQueueManager> queue_manager;
   yasper::ptr<IMessageQueue> cola_cargadores_equipaje;
   // yasper::ptr<IMessageQueue> cola_asignaciones;
	CintaCentral * cinta;

	int id_productor;
	MENSAJE_VUELO_ENTRANTE mensaje;
	static const int cant_ipc = 3;

public:

	void poner_en_cinta_principal(const Equipaje& equipaje);
	// int esperar_zona_asignada(int numero_vuelo);
	int esperar_vuelo_entrante(int numero_vuelo_destino);

	ApiTrasbordo(const char* directorio_de_trabajo, const char* config_file, int numero_de_vuelo);
	virtual ~ApiTrasbordo();

};

#endif
