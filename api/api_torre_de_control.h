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

// class AsignacionZona;
// class ZonasAsignadas;

class ApiTorreDeControl {
// private:
   // SemaphoreSet sem_set;
   // Mutex mutex_asignaciones;
   // SharedObject<ZonasAsignadas> asignaciones;

public:
   ApiTorreDeControl(const char *directorio_de_trabajo, const char* config_file);
   virtual ~ApiTorreDeControl() {};
   void notificar_llegada_vuelo(int numero_vuelo);

   void pedir_contenedor();
   void liberar_contenedor();

   int pedir_puesto_checkin(int num_vuelo);
   void liberar_puesto_checkin(int num_puesto_checkin);

   int pedir_zona(int num_vuelo);
   void liberar_zona(int num_zona);

   /*
    * Guarda el estado de la asignacion de zonas en shared memory.
    **/
   // void asignar_vuelo(int zona, int vuelo);
   // void desasignar_vuelo(int zona);
   // int get_zona(int num_vuelo);

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

// class AsignacionZona {
// private:
//    int num_zona;
//    int num_vuelo;
// public:
//    AsignacionZona():num_zona(0), num_vuelo(0) {
//    }

//    AsignacionZona(int zona, int vuelo):num_zona(zona), num_vuelo(vuelo) {
//    }

//    int zona() const { return num_zona; }
//    int vuelo() const { return num_vuelo; }
//    int get_zona(int num_vuelo);
// };

// class ZonasAsignadas {
// public:
//    ZonasAsignadas() : cant_zonas_asignadas(0) {
//    }

//    void asignar_vuelo(int zona, int vuelo) {
//       if(cant_zonas_asignadas == MAX_ZONAS)
//          throw std::runtime_error("Maximo de zonas asignadas");

//       zonas[cant_zonas_asignadas] = AsignacionZona(zona, vuelo);
//       cant_zonas_asignadas++;
//       Log::info("ApiDespachante zona asignada zona=%d vuelo=%d cant=%d", zonas[cant_zonas_asignadas-1].zona(),
//                 zonas[cant_zonas_asignadas-1].vuelo(), cant_zonas_asignadas);
//    }

//    void desasignar_vuelo(int vuelo) {
//       int eliminados = 0;
//       for (int i=0; i<cant_zonas_asignadas; i++) {
//          if(zonas[i].vuelo() == vuelo)
//             eliminados++;
//          else
//             zonas[i-eliminados] = zonas[i];
//       }
//    }

//    int get_zona(int num_vuelo) const {
//       int zona = -1;
//       int i = 0;
//       while( (zona==-1) && (i<cant_zonas_asignadas)) {
//          Log::info("ApiDespachante get_zona(num_vuelo=%d): zona=%d vuelo =%d", num_vuelo, zonas[i].zona(), zonas[i].vuelo());
//          if(zonas[i].vuelo() == num_vuelo) {
//             zona = zonas[i].zona();
//          }
//          i++;
//       }
//       return zona;
//    }

// private:
//    int cant_zonas_asignadas;
//    AsignacionZona zonas[MAX_ZONAS];
// };

#endif
