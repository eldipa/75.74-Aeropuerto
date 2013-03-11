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

class AsignacionZona;
class ZonasAsignadas;

class ApiTorreDeControlSvc {
private:
   SemaphoreSet sem_set;
   Mutex mutex_asignaciones;
   SharedObject<ZonasAsignadas> asignaciones;
   // std::string path_torre_control;

public:
   ApiTorreDeControlSvc(const char *directorio_de_trabajo, const char* config_file);
   virtual ~ApiTorreDeControlSvc() {};

   /*
    * Guarda el estado de la asignacion de zonas en shared memory.
    **/
   void asignar_vuelo(int zona, int vuelo);
   void desasignar_vuelo(int zona);
   int get_zona(int num_vuelo);

};


class ZonasAsignadas {
public:
   ZonasAsignadas() {
      for(int i=0; i<MAX_ZONAS; i++) {
         zonas[i] = -1;
      }
   }

   void asignar_vuelo(int zona, int vuelo) {
      if((zona<=0) || (zona >= MAX_ZONAS))
         throw std::runtime_error("Numero de zona inválido");

      zonas[zona-1] = vuelo;

      Log::info("ApiDespachante zona asignada zona=%d vuelo=%d", zona, vuelo);
   }

   void desasignar_vuelo(int vuelo) {
      for( int i=0; i<MAX_ZONAS; i++ ) {
         if(zonas[i] == vuelo) {
            zonas[i] = -1;
         }
      }

      Log::info("ZonasAsignadas zona desasignada  vuelo=%d",  vuelo);
   }

   int get_zona(int num_vuelo) const {
      int zona = -1;
      int i = 0;

      while( (zona==-1) && (i<MAX_ZONAS)) {

         if(zonas[i] != -1)
            Log::info("ZonasAsignadas get_zona(num_vuelo=%d): zona=%d vuelo =%d", num_vuelo, i, zonas[i]);

         if(zonas[i] == num_vuelo) {
            zona = i;
         }
         i++;
      }
      return zona;
   }

private:
   int zonas[MAX_ZONAS];
};

#endif
