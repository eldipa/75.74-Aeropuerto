#ifndef APICHECKIN_H_
#define APICHECKIN_H_

#include "equipaje.h"
#include "semaphoreset.h"
#include "mutex.h"
#include "messagequeue.h"
#include <memory>
#include "sharedobject.h"

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
} tMeansajeCheckin;

class PuestoCheckinSinVueloAsignado {
private:
   int id_checkin;
public:
   PuestoCheckinSinVueloAsignado(int id_checkin):id_checkin(id_checkin) {
   }
};

class ApiCheckIn {
public:

   ApiCheckIn(int id_checkin, const char* path_to_locks, int id_cinta_checkin);
   virtual ~ApiCheckIn();
   
   /*
    * Asigna un vuelo al puesto de checkin.
    **/
   void iniciar_checkin( int numero_vuelo );
    
   /*
    * Cierra el checkin iniciado con #iniciar_checkin 
    * El checkin debe haber sido abi
    **/
   void cerrar_checkin();
   
   /*
    * Bloquea hasta que llegue un mensaje de iniciar_checkin
    */
   void esperar_inicio_checkin();

   /*
    * Bloquea hasta que llegue un mensaje de controlador_de_checkin
    */
   void recibir_mensaje_controlador_checkin(tMeansajeCheckin&);
   
   /*
    *  Registra un equipaje que hace checkin.
    *  A partir de aca el avion no sale hasta que llega el equipaje
    **/
   void registrar_equipaje( Equipaje& );
   
   /*
    * Retorna el numero_vuelo que esta haciendo checkin en ese momento.
    * Si no se esta haciendo checkin en ese momento lanza la excepcion
    * PuestoCheckinSinVueloAsignado
    **/
   int get_vuelo_actual();

   /*
    * Indica que hay un checkin de pasajero en curso.
    * hasta que se llame a fin_checkin_pasajero
    **/
   void comienza_checkin_pasajero();

   /*
    * Llamar despues de registrar los equipajes de un pasajero
    *
    **/
   void fin_checkin_pasajero();

   void recibir_pasajero_para_checkin(int& id_pasajero, std::vector<Equipaje>& equipajes);

   void llego_pasajero_para_checkin(int id_pasajero, const std::vector<Equipaje>& equipajes);
private:
   static const int cant_ipcs = 3; 

   int id_checkin;
   char path_to_torre_de_control_lock[128];
   char path_to_cinta_checkin_lock[128];
   char path_to_puesto_checkin_lock[128];
   char path_to_control_checkin_lock[128];
   int id_cinta_checkin;

   std::auto_ptr<SemaphoreSet> sem_set;
   std::auto_ptr<Mutex> mutex_checkin;
   std::auto_ptr<MessageQueue> queue_pasajeros;
   std::auto_ptr< SharedObject<int> > vuelo_actual;
};

#endif /* APICHECKIN_H_ */
