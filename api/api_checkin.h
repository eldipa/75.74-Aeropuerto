#ifndef APICHECKIN_H_
#define APICHECKIN_H_

#include "equipaje.h"
#include "semaphoreset.h"
#include "mutex.h"
#include <memory>

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
    * llamar antes de hacer el checkin de un pasajero.
    * (antes de llamar a registrar_equipaje)
    **/
   void comienza_checkin_pasajero();
   
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
    * Llamar despues de registrar los equipajes de un pasajero
    *
    **/
   void fin_checkin_pasajero();

   void recibir_pasajero_para_checkin(int& id_pasajero, std::vector<Equipaje>& equipajes);
private:
   
   int id_checkin;
   char path_to_torre_de_control_lock[128];
   char path_to_cinta_checkin_lock[128];
   char path_to_puesto_checkin_lock[128];
   int id_cinta_checkin;
   int vuelo_actual;

   std::auto_ptr<SemaphoreSet> sem_set;
   std::auto_ptr<Mutex> mutex_checkin;
};

#endif /* APICHECKIN_H_ */
