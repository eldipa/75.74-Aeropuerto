#ifndef _API_CONFIGURACION_H_
#define _API_CONFIGURACION_H_

#include "iqueue_manager.h"
#include "ipc_queue_manager.h"
#include "broker_queue_manager.h"

class ApiConfiguracion {
public:
   
   static yasper::ptr<IQueueManager> get_queue_manager(const char* directorio_de_trabajo) {
      //return new IpcQueueManager(directorio_de_trabajo); // Funciona con colas del so como en la primer entrega
      return new BrokerQueueManager(directorio_de_trabajo); // Funciona con colas del so pero accedidas a travez de un broker en shared memory
      //return new BrokerQueueManager("localhost", 7000)
   }
};

#endif
