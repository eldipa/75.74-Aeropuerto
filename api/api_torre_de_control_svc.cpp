#include "api_torre_de_control_svc.h"
#include "api_constants.h"

#include "database.h"
#include "stmt.h"
#include "tupleiter.h"

#include "semaphoreset.h"
#include "mutex.h"
#include "process.h"

#include "log.h"

#include <stdexcept>
#include <cstdio>
#include <cstring>
#include <string>
#include "yasper.h"

#include "api_configuracion.h"


ApiTorreDeControlSvc::ApiTorreDeControlSvc(const char* directorio_de_trabajo, const char* config_file) :
   sem_set(std::string(directorio_de_trabajo).append(PATH_TORRE_DE_CONTROL_SVC).c_str(), 0, 1),
   mutex_asignaciones(sem_set, 0),
   asignaciones(std::string(directorio_de_trabajo).append(PATH_TORRE_DE_CONTROL_SVC).c_str(), 1) {

   config_file = config_file;
}

void ApiTorreDeControlSvc::asignar_vuelo(int zona, int vuelo) {
   Log::info("Torre de control: asignando zona %d a vuelo %d", zona, vuelo);

   mutex_asignaciones.lock();
   asignaciones->asignar_vuelo(zona, vuelo);
   mutex_asignaciones.unlock();   
}

void ApiTorreDeControlSvc::desasignar_vuelo(int num_vuelo) {
   Log::info("ApiTorreDeControlSvc: desasignando vuelo %d",  num_vuelo);
   
   mutex_asignaciones.lock();
   asignaciones->desasignar_vuelo(num_vuelo);
   mutex_asignaciones.unlock();   
}

int ApiTorreDeControlSvc::get_zona(int num_vuelo) {
   Log::info("ApiTorreDeControlSvc obtuve resultado del rpc server: zona del vuelo %d", num_vuelo);

   mutex_asignaciones.lock();
   int zona = asignaciones->get_zona(num_vuelo);
   mutex_asignaciones.unlock();
   return zona;
   
}
