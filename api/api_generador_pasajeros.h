#ifndef APIGENERADORPASAJEROS_H_
#define APIGENERADORPASAJEROS_H_

#include "equipaje.h"

#include "iqueue_manager.h"
#include "imessagequeue.h"

#include "yasper.h"
#include <vector>
 
class ApiGeneradorPasajeros {
public:

   ApiGeneradorPasajeros(const char* directorio_de_trabajo, const char* config_file, bool create);
   ApiGeneradorPasajeros(const char* directorio_de_trabajo, const char* conig_file);
   virtual ~ApiGeneradorPasajeros();

   void llego_pasajero_para_checkin(int id_pasajero, int id_puesto_checkin, const std::vector<Equipaje>& equipajes);

private:
   yasper::ptr<IQueueManager> queue_manager;
   yasper::ptr<IMessageQueue> queue_pasajeros;
};

#endif
