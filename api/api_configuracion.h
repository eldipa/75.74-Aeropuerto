#ifndef _API_CONFIGURACION_H_
#define _API_CONFIGURACION_H_

#include "iqueue_manager.h"
#include "ipc_queue_manager.h"
#include "broker_queue_manager.h"

class ApiConfiguracion {
public:
   
   static yasper::ptr<IQueueManager> get_queue_manager(const char* directorio_de_trabajo, bool create = false) {
      directorio_de_trabajo = directorio_de_trabajo;
      create = create;

      // Funciona con colas del so como en la primer entrega
      //return new IpcQueueManager(directorio_de_trabajo);

      // Funciona con colas del so pero accedidas a travez de un broker en shared memory (para debug)
      // if( create )
      //    return new BrokerQueueManager( new MessageBroker(directorio_de_trabajo, create) );
      // else
      //    return new BrokerQueueManager( new MessageBroker(directorio_de_trabajo) );

      //accede a las colas a travez de un broker remoto
      return new BrokerQueueManager( new MessageBrokerStub("localhost", "9000") );
   }
};

#endif
