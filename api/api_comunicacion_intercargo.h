#ifndef APICOMUNICACIONINTERCARGO_H_
#define APICOMUNICACIONINTERCARGO_H_

#include "mensajes.h"
#include "messagequeue.h"

#include "yasper.h"
#include "imessagequeue.h"
#include "iqueue_manager.h"

class ApiComunicacionIntercargo {
private:

	MENSAJE_VUELO_ENTRANTE mensaje;

   yasper::ptr<IQueueManager> queue_manager;
   yasper::ptr<IMessageQueue> cola_cargadores_equipaje;

public:
	ApiComunicacionIntercargo(const char * directorio_de_trabajo);
	virtual ~ApiComunicacionIntercargo();

	void informar_vuelo_entrante(int numero_vuelo_entrante, int numero_vuelo_destino);
};

#endif /* APICOMUNICACIONINTERCARGO_H_ */
