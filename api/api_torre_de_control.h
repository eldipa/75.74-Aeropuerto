#ifndef API_TORRE_DE_CONTROL_H_
#define API_TORRE_DE_CONTROL_H_

#include "messagequeue.h"
#include <string>

#include "yasper.h"
#include "imessagequeue.h"
#include "iqueue_manager.h"

#include "semaphoreset.h"
#include "mutex.h"
#include "sharedobject.h"

#include "api_constants.h"

#include <stdexcept>
#include "log.h"

class ApiTorreDeControl {

public:
   ApiTorreDeControl(const char *directorio_de_trabajo, const char* config_file);
   ApiTorreDeControl(const char *directorio_de_trabajo, const char* config_file, bool create);

   virtual ~ApiTorreDeControl() {};
   void notificar_llegada_vuelo(int numero_vuelo);

   void pedir_contenedor();
   void liberar_contenedor();

   int pedir_puesto_checkin(int num_vuelo);
   void liberar_puesto_checkin(int num_puesto_checkin);

   int pedir_zona(int num_vuelo);
   void liberar_zona(int num_zona);

private:
   typedef struct t_msg_contenedores {
      long mtype;
   } tMsgContenedores;

   typedef struct t_msg_checkin {
      long mtype;
      int puesto_checkin;
   } tMsgCheckin;

   typedef struct t_msg_zona {
      long mtype;
      int num_zona;
   } tMsgZona;


   std::string path_torre_control;

   yasper::ptr<IQueueManager> queue_manager;
   yasper::ptr<IMessageQueue> queue_zonas;
   yasper::ptr<IMessageQueue> queue_puestos_checkin;
   yasper::ptr<IMessageQueue> queue_contenedores;

};


#endif
