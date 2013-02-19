#ifndef _API_CONFIGURACION_H_
#define _API_CONFIGURACION_H_

#include "iqueue_manager.h"
#include "ipc_queue_manager.h"
#include "broker_queue_manager.h"
#include "genericerror.h"

#include "iniparser.h"
#include <string>

class ApiConfiguracion {
private:
   static std::string wkdir;

public:
   
   // static yasper::ptr<IQueueManager> get_queue_manager(const char* directorio_de_trabajo, const char* config_file, bool create = false) {
   static yasper::ptr<IQueueManager> get_queue_manager(const char* directorio_de_trabajo, bool create = false) {
   //static yasper::ptr<IQueueManager> get_queue_manager(const char* directorio_de_trabajo, const char* config_file, bool create = false) {
      // config_file = config_file;
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
   
    static std::string get_wkdir(const char* archivo_configuracion) {
       if(ApiConfiguracion::wkdir == "") {
          ApiConfiguracion::parse_config_file(archivo_configuracion);
       } 
       return ApiConfiguracion::wkdir;
    }

private:

    static void parse_config_file(const char * config_file) {
       dictionary * ini = iniparser_load(config_file);

       if (ini==NULL) {
          Log::crit("cannot parse the config file: %s\n", config_file);
          throw GenericError("cannot parse the config file %s", config_file);
       }

       ApiConfiguracion::wkdir = iniparser_getstring(ini, "AEROPUERTO:working_dir", NULL);
       Log::info("ApiConfiguracion: ok al leer wkdir %s", wkdir.c_str());
       iniparser_freedict(ini);
    }

};


#endif
