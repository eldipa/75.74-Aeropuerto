#ifndef APICHECKIN_H_
#define APICHECKIN_H_

#include "equipaje.h"
#include "semaphoreset.h"
#include "mutex.h"
#include <memory>
#include "sharedobject.h"
#include "cintas.h"

#include "yasper.h"
#include "iqueue_manager.h"
#include "imessagequeue.h"

#define MAX_EQUIPAJES_POR_PERSONA 5

typedef struct t_msg_pasajero {
   long mtype;// id_checkin
   int id_pasajero;
   int cant_equipajes;
   Equipaje equipajes[MAX_EQUIPAJES_POR_PERSONA];
} tMensajePasajeroCheckin;


typedef struct t_msg_checkin {
   long mtype;// id_puesto_checkin
   bool iniciar_checkin; //false --> cerrar_checkin
   int num_vuelo;
   int num_zona;
} tMeansajeCheckin;

typedef struct vuelo_en_checkin {
   int num_vuelo;
   int cant_equipajes;
   int id_cinta_checkin;
   vuelo_en_checkin():num_vuelo(-1), cant_equipajes(0), id_cinta_checkin(-1) {
   }
   vuelo_en_checkin(int id_cinta_checkin):num_vuelo(-1), cant_equipajes(0), id_cinta_checkin(id_cinta_checkin) {
   }
} tVueloEnCheckin;

class PuestoCheckinSinVueloAsignado {
private:
   int id_checkin;
public:
   PuestoCheckinSinVueloAsignado(int id_checkin):id_checkin(id_checkin) {
   }
};

class ApiCheckIn {
public:

   ApiCheckIn(const char* directorio_de_trabajo, const char* conig_file, int id_checkin );
   ApiCheckIn(const char* directorio_de_trabajo, const char* config_file, int id_checkin, int id_cinta_checkin, bool create );

   virtual ~ApiCheckIn();

   /*
    * puesto_checkin
    * Asigna un vuelo al puesto de checkin.
    **/
   void iniciar_checkin( int numero_vuelo );

   /*
    * puesto_checkin
    * Cierra el checkin iniciado con #iniciar_checkin
    * El checkin debe haber sido abi
    **/
   int cerrar_checkin();

   /*
    * puesto_checkin
    * Bloquea hasta que llegue un mensaje de controlador_de_checkin
    */
   void recibir_mensaje_controlador_checkin(tMeansajeCheckin&);

   /*
    * puesto_checkin
    *  Registra un equipaje que hace checkin.
    *  A partir de aca el avion no sale hasta que llega el equipaje
    **/
   void registrar_equipaje( Equipaje& );

   /*
    * puesto_checkin
    * Retorna el numero_vuelo que esta haciendo checkin en ese momento.
    * Si no se esta haciendo checkin en ese momento lanza la excepcion
    * PuestoCheckinSinVueloAsignado
    **/
   int get_vuelo_actual();

   /*
    * puesto_checkin
    * Indica que hay un checkin de pasajero en curso.
    * hasta que se llame a fin_checkin_pasajero
    **/
   void comienza_checkin_pasajero();

   /*
    * puesto_checkin
    * Llamar despues de registrar los equipajes de un pasajero
    *
    **/
   void fin_checkin_pasajero();

   //puesto_checkin
   void recibir_pasajero_para_checkin(int& id_pasajero, std::vector<Equipaje>& equipajes);

   int get_cinta_checkin();

private:
   static const int cant_ipcs = 2;

   std::string path_to_locks;
   int id_checkin;
   SharedObject<tVueloEnCheckin> vuelo_actual;
   CintaCheckin cinta_checkin_out;
   SemaphoreSet sem_set;
   Mutex mutex_checkin;
   yasper::ptr<IQueueManager> queue_manager;
   yasper::ptr<IMessageQueue> queue_pasajeros;


};

#endif /* APICHECKIN_H_ */
