#ifndef CINTA_H_
#define CINTA_H_

#include "sharedmemory.h"
#include "boundedqueue.h"
#include "semaphoreset.h"
#include "mutex.h"
#include <vector>
#include "sharedmemory.h"

#include <string.h>
#include <stdio.h>
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
   Cinta( const char* absolute_path, int num_cinta, bool create )
      : waiting_sem( std::vector<short unsigned int>(5, 1), absolute_path, num_cinta * cant_ipcs ),
        shm( absolute_path, (num_cinta*cant_ipcs)+1, sizeof(BoundedQueue<T,N>)+(sizeof(bool)*(2)), 0664, create ),
        mutex_cinta( waiting_sem, 0),
        mutex_lleno( waiting_sem, 1 ),
        mutex_vacio( waiting_sem, 2 ),
        mutex_reader( waiting_sem, 3 ),
        mutex_writer( waiting_sem, 4 ) {

      setup_name(absolute_path, num_cinta);

      Log::debug("Creando %s", name);

      // Asignar punteros
      cinta = (BoundedQueue<T,N>*)shm.memory_pointer();
      reader_waiting = (bool*)(cinta + 1);
      writer_waiting = (bool*)(reader_waiting + 1);

      // Inicializar (al final)
      new(cinta) BoundedQueue<T,N>();
      reader_waiting = false;
      writer_waiting = false;

      mutex_lleno.lock();
      mutex_vacio.lock();
   }

   Cinta( const char* absolute_path, int num_cinta )
      : waiting_sem( absolute_path, num_cinta * cant_ipcs, 5 ),
        shm( absolute_path, (num_cinta*cant_ipcs)+1, sizeof(BoundedQueue<T,N>)+(sizeof(bool)*(2)),0664),
        mutex_cinta( waiting_sem, 0),
        mutex_lleno( waiting_sem, 1 ),
        mutex_vacio( waiting_sem, 2 ),
        mutex_reader( waiting_sem, 3 ),
        mutex_writer( waiting_sem, 4 ) {

      setup_name(absolute_path, num_cinta);

      Log::debug("Conectándose a %s", name);

      // Asignar punteros
      cinta = (BoundedQueue<T,N>*)shm.memory_pointer();
      reader_waiting = (bool*)(cinta + 1);
      writer_waiting = (bool*)(reader_waiting + 1);
   }

   /*
    * Saca un equipaje de la cinta.
    * Si la cinta esta vacia, se bloquea hasta que haya un equipaje disponible
    **/
   T sacar_equipaje() {
      mutex_reader.lock();
      mutex_cinta.lock();

      while( cinta->is_empty() ) {
         wait_no_vacio();
      }

      if(cinta->is_full()) {
         unlock_productores();
      }

      T return_element = cinta->take();

      mutex_cinta.unlock();
      mutex_reader.unlock();

      return return_element;
   }

   /**/
   /*
    * Pone un equipaje de la cinta.
    * Si la cinta esta llena, se bloquea hasta que haya lugar
    **/
   void poner_equipaje( T elemento ) {
      mutex_writer.lock();
      mutex_cinta.lock();

      while(cinta->is_full()) {
         wait_no_lleno();
      }
      if( cinta->is_empty()) {
         unlock_consumidores();
      }
      cinta->offer( elemento );

      mutex_cinta.unlock();
      mutex_writer.unlock();
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

   char name[80];
   bool* reader_waiting;
   bool* writer_waiting;
   BoundedQueue<T,N>* cinta;

   SemaphoreSet waiting_sem;
   SharedMemory shm;

   Mutex mutex_cinta;
   Mutex mutex_lleno;
   Mutex mutex_vacio;
   Mutex mutex_reader;
   Mutex mutex_writer;

   static const int cant_ipcs = 2;
   void wait_no_lleno() {
      Log::debug("%s llena :(", name);
      *writer_waiting = true;
      mutex_cinta.unlock();
      mutex_lleno.lock();
      mutex_cinta.lock();
   }

   void wait_no_vacio() {
      *reader_waiting = true;
      Log::debug("%s vacía :(", name);
      mutex_cinta.unlock();
      mutex_vacio.lock();
      mutex_cinta.lock();
   }
  void unlock_productores() {
    if (*writer_waiting) {
      Log::debug("%s dejó de estar llena :)", name);
      mutex_lleno.unlock();
      *writer_waiting = false;
    }
  }

  void unlock_consumidores() {
    if (*reader_waiting) {
      Log::debug("%s dejó de estar vacía :)", name);
      mutex_vacio.unlock();
      *reader_waiting = false;
    }
  }

  void setup_name(const char *path, int num_cinta) {
    const char* last_slash = strrchr(path, '/');
    if (last_slash)
      snprintf(name, 80, "Cinta(%s:%i)", last_slash + 1, num_cinta);
    else
      snprintf(name, 80, "Cinta(%s:%i)", path, num_cinta);
  }
};

#endif
