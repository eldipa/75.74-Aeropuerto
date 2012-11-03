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
// T: Tipo almacenado
// N: Máximo de elementos en la cinta
// MP: Máximo de productores
// MC: Máximo de consumidores
template <class T, int N, int MP, int MC>
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
   Cinta( const char* absolute_path_mem, const char* absolute_path_sem, bool create )
      : mutex_cinta( waiting_sem, MP+MC ),
        waiting_sem( std::vector<short unsigned int>(MP+MC+1, 0), absolute_path_sem ),
        shm( absolute_path_mem, sizeof(BoundedQueue<T,N>)+(sizeof(bool)*(MP+MC)),0664, create ) {

      // Asignar punteros
      cinta = (BoundedQueue<T,N>*)shm.memory_pointer();
      is_waiting = (bool*)(cinta + 1);

      // Inicializar (al final)
      new(cinta) BoundedQueue<T,N>();
      for(unsigned int i=0; i<MP+MC; i++)
         is_waiting[i] = false;
      mutex_cinta.unlock();
   }

   Cinta( const char* absolute_path_mem, const char* absolute_path_sem )
      : mutex_cinta( waiting_sem, MP + MC ),
        waiting_sem( absolute_path_sem, MP + MC + 1 ),
        shm( absolute_path_mem, sizeof(BoundedQueue<T,N>)+(sizeof(bool)*(MP+MC)),0664) {

      // Asignar punteros
      cinta = (BoundedQueue<T,N>*)shm.memory_pointer();
      is_waiting = (bool*)(cinta + 1);
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

   friend std::ostream& operator<<( std::ostream& stream,  Cinta<T,N, MP, MC>& q ) {
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
      for( unsigned int i=0; i< MP; i++ ) {
         if (get_is_waiting(i, true)) {
           get_is_waiting(i, true) = false;
           get_sem(i,true).unlock();
         }
      }
   }

   void unlock_consumidores() {
      for( unsigned int i=0; i< MC; i++ ) {
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
         return Mutex(waiting_sem, id+MP);
   }

   bool& get_is_waiting( unsigned int id, bool es_productor ) {
      if( es_productor )  {
         return is_waiting[id];
      } else {
         return is_waiting[id+MP];
      }
   }

};

#endif
