#ifndef CINTA_PRINCIPAL_H_
#define CINTA_PRINCIPAL_H_

#include "sharedobject.h"
#include "semaphoreset.h"

/*
Como usar la interfaz CintaPrincipal
====================================

Para alguno de los robots con id rid y con un robot vecino con id vid
---------------------------------------------------------------------

valija = obtener_valija_para(rid);
es algo para mi?
   listo_para_recibir_valija_para(rid);
   .....algunos etc con 'valija'....
   volver al principio
no, no es para mi
   mover_valija(rid, vid)
   volver al principio


Para el proceso que pone valijas en el primer proceso rid
---------------------------------------------------------
poner_valija(rid)


Para el robot que descarta cosas
--------------------------------

valija = obtener_valija_para(rid);
listo_para_recibir_valija_para(rid);
.....algunos etc con 'valija'....
volver al principio

*/

template<class T, int CantRobots>
class CintaPrincipal {
   private:
      struct _Cinta {
         T posiciones[CantRobots];
      };

      SharedObject<struct _Cinta> cinta;
      SemaphoreSet vacio;
      SemaphoreSet lleno;

   public:
      CintaPrincipal(bool, const char *abs_path_memory, const char *abs_path_sem_vacio, const char* abs_path_sem_lleno) :
         cinta(abs_path_memory, 0664, true),
         vacio(std::vector<unsigned short>(CantRobots, 1), abs_path_sem_vacio),
         lleno(std::vector<unsigned short>(CantRobots, 0), abs_path_sem_lleno) {
         }
      
      CintaPrincipal(const char *abs_path_memory, const char *abs_path_sem_vacio, const char* abs_path_sem_lleno) :
         cinta(abs_path_memory),
         vacio(abs_path_sem_vacio, CantRobots),
         lleno(abs_path_sem_lleno, CantRobots) {
         }
      
      /*
       * El proceso se bloquea a la espera de que llege una valija
       * para el robot_id.
       *
       * La valija es retornada.
       * Sera responsabilidad del usuario el mantener una copia del
       * objeto retornado.
       *
       * Luego de ejecutar este metodo, se puede llamar a:
       *    - mover_valija
       *    - listo_para_recibir_valija_para
       * De forma excluyente.
       * */
      const T& obtener_valija_para(int robot_id) {
         lleno.wait_on(robot_id);
         return cinta->posiciones[robot_id];
      }

      /*
       * Mueve la valija que tiene el robot_id origen (desde_robot_id)
       * y se la pasa a otro robot (hasta_robot_id).
       *
       * El usuario es responsable de haber llamado previamente a 'obtener_valija_para'
       * y de no haber llamado a 'listo_para_recibir_valija_para'.
       *
       * El metodo se bloqueara a la espera de que el robot destino este libre
       * para recibir la valija.
       * Automaticamente esta llamada habilita que otros procesos envien una nueva valija
       * que debera ser obtenido con 'obtener_valija_para'
       *
       **/
      void mover_valija(int desde_robot_id, int hasta_robot_id) {
         esperar_que_este_libre(hasta_robot_id);
         cinta->posiciones[hasta_robot_id] = cinta->posiciones[desde_robot_id];
         avisar_que_hay_valija_para(hasta_robot_id);
         listo_para_recibir_valija_para(desde_robot_id);
      }
      
      /*
       * Luego de llamar a 'obtener_valija_para', puede que NO se requiera 
       * que la valija se mueva a otro proceso.
       * En este caso es necesario que el robot ya ha liberado el espacio
       * y que otro proceso puede entregarnos una nueva valija.
       * 
       * Para esto, se debe ejecutar 'listo_para_recibir_valija_para'
       * Una vez ejecutado este metodo, el siguiente metodo que puede
       * ejecutarse es 'obtener_valija_para'.
       * */
      void listo_para_recibir_valija_para(int robot_id) {
         vacio.signalize(robot_id);
      }
      
      /*
       * Este metodo funciona de forma separada al resto de los metodos en esta
       * interfaz.
       *
       * Esta pensada para procesos 'productores' que ponen nuevas valijas en el
       * sistema.
       *
       * Este metodo copia la valija 'e' y se le entrega al robot_id esperando 
       * que el robot tenga espacio para recibir la nueva valija.
       * */
      void poner_valija(const T& e, int robot_id) {
         esperar_que_este_libre(robot_id);
         cinta->posiciones[robot_id] = e;
         avisar_que_hay_valija_para(robot_id);
      }
      
   private:
      void esperar_que_este_libre(int robot_id) {
         vacio.wait_on(robot_id);
      }

      void avisar_que_hay_valija_para(int robot_id) {
         lleno.signalize(robot_id);
      }
};

  

#endif
