#ifndef CINTA_H_
#define CINTA_H_

#include "sharedmemory.h"
#include "boundedqueue.h"
#include "semaphoreset.h"
#include "mutex.h"
#include <vector>
#include "sharedmemory.h"

#include <iostream>
#include <unistd.h>
#include "log.h"

/*
 * Cinta que puede tener hasta N elementos de tipo T. 
 **/
template <class T, int N>
class Cinta {
   
public:

   /*
    * Crea una cinta en shared memory.Por ahora no se tiene en cuenta el parametro 'create'.
    * Si la cinta ya existe hay que llamar al segundo constructor.
    *
    * los parametros 'productores' y 'consumidores' son la cantidad de productores y consumidores
    * que acceden a la cinta.Los productores a travez de poner_equipaje y los consumidores con sacar_equipaje. 
    * Cada productor y consumidor tiene un id para acceder a la cinta: (0..productores-1) y (0..consumidores-1)
    * Es importante que cada proceso use siempre el mismo id.
    *
    * absolute_path_mem y absolute_path_sem son los path utilizados para crear el set de semaforos y la shared memory.
    * Tienen que ser distintos.
    **/
   Cinta( unsigned int productores, unsigned int consumidores, const char* absolute_path_mem, const char* absolute_path_sem, bool create )
      : mutex_cinta( waiting_sem, productores+consumidores ),
        waiting_sem( std::vector<short unsigned int>(productores+consumidores+1, 0), absolute_path_sem ),
        shm( absolute_path_mem, sizeof(BoundedQueue<T,N>)+(sizeof(bool)*(productores+consumidores)),0664, create ) {

      cinta = (BoundedQueue<T,N>*)shm.memory_pointer();
      (*cinta) = BoundedQueue<T,N>();
      cant_productores = productores;
      cant_consumidores = consumidores;

      is_waiting = (bool*)shm.memory_pointer()+sizeof(BoundedQueue<T,N>);
      for(unsigned int i=0; i<productores+consumidores; i++)
         is_waiting[i] = false;
      mutex_cinta.unlock();
   }

   Cinta( unsigned int productores, unsigned int consumidores, const char* absolute_path_mem, const char* absolute_path_sem )
      : mutex_cinta( waiting_sem, productores+consumidores ),
        waiting_sem( absolute_path_sem, productores+consumidores+1 ),
        shm( absolute_path_mem, sizeof(BoundedQueue<T,N>)+(sizeof(bool)*(productores+consumidores)),0664) {

      cinta = (BoundedQueue<T,N>*)shm.memory_pointer();
      cant_productores = productores;
      cant_consumidores = consumidores;

      is_waiting = (bool*)shm.memory_pointer()+sizeof(BoundedQueue<T,N>);
      for(unsigned int i=0; i<productores+consumidores; i++)
         is_waiting[i] = false;
      mutex_cinta.unlock();
   }

   /*
    * Saca un equipaje de la cinta.
    * Si la cinta esta vacia, se bloquea hasta que haya un equipaje disponible
    **/
   T sacar_equipaje( unsigned int id_consumidor ) {
      mutex_cinta.lock();
      while( cinta->is_empty() ) {
         wait_no_vacio( id_consumidor );
      }
      if(cinta->is_full()) {
         unlock_productores();
      }
      T return_element = cinta->take();
      mutex_cinta.unlock();
      return return_element;
   }

   /**/
   /*
    * Pone un equipaje de la cinta.
    * Si la cinta esta llena, se bloquea hasta que haya lugar
    **/
   void poner_equipaje( T elemento, unsigned int id_productor ) {
      mutex_cinta.lock();
      while(cinta->is_full()) {
         wait_no_lleno( id_productor );
      }
      if( cinta->is_empty()) {
         unlock_consumidores();
      }
      cinta->offer( elemento );
      mutex_cinta.unlock();
   }

   virtual ~Cinta() {
   }

   friend std::ostream& operator<<( std::ostream& stream,  Cinta<T,N>& q ) {
      q.mutex_cinta.lock();
      stream << (*q.cinta);
      q.mutex_cinta.unlock();
      return stream;
   }

private:
   Cinta(const Cinta&);
 
   bool* is_waiting;
   Mutex mutex_cinta;
   SemaphoreSet waiting_sem;
   unsigned int cant_productores;
   unsigned int cant_consumidores;
   BoundedQueue<T,N>* cinta;   
   SharedMemory shm;

   void wait_no_lleno( unsigned int id_productor ) { 
      get_is_waiting(id_productor, true) = true;
      mutex_cinta.unlock();
      get_sem(id_productor, true).lock();
      mutex_cinta.lock();
   }

   void wait_no_vacio( unsigned int id_consumidor ) {
      get_is_waiting(id_consumidor, false) = true;
      mutex_cinta.unlock();
      get_sem(id_consumidor, false).lock();
      mutex_cinta.lock();
   }

   void unlock_productores() {
      for( unsigned int i=0; i<cant_productores; i++ ) {
         if (get_is_waiting(i, true)) {
           get_is_waiting(i, true) = false;
           get_sem(i,true).unlock();
         }
      }
   }

   void unlock_consumidores() {
      for( unsigned int i=0; i<cant_consumidores; i++ ) {
        if (get_is_waiting(i, false)) {
          get_is_waiting(i, false) = false;
          get_sem(i,false).unlock();
        }
      }      
   }

   Mutex get_sem( unsigned int id, bool es_productor ) {
      if( es_productor )
         return Mutex(waiting_sem, id);
      else 
         return Mutex(waiting_sem, id+cant_productores);
   }

   bool& get_is_waiting( unsigned int id, bool es_productor ) {
      if( es_productor )  {
         return is_waiting[id];
      } else {
         return is_waiting[id+cant_productores];
      }
   }

};

#endif
