#ifndef API_CARGA_H_
#define API_CARGA_H_

#include "equipaje.h"
#include "contenedor.h"

class ApiCarga {
private:
   std::string path;
   int id_robot_carga;

public:
   ApiCarga(const char* path_carga, int id_robot_carga):path(path_carga),id_robot_carga(id_robot_carga) {
   }

   /*
    * Bloquea hasta que el robot carga cargue n equipaejes en contenedores.
    * No bloquea y retorna false si el robot_carga esta ocupado en otra carga.
    * El robot carga permanece ocupado hasta que el controlador llame a 
    * liberar_robot_carga;
    **/
   bool cargar_equipajes(int n) {
      return (n>0);
   }

   /*
    * Bloquea hasta que el controlador de carga llame a cargar_equipajes.
    * Devuelve la cantidad de equipajes que se deben cargar.
    **/
   int get_equipajes_por_cargar() {
      return 10;
   }

   /*
    * Guarda el contenedor con equipajes recibidos por el robot_carga.
    * Se accede mediante get_contenedor(int).
    **/
   void contenedor_cargado(Contenedor& c) {
      c.get_rfids();
   }

   /*
    * Desbloquea a controlador de equipajes parado en cargar_equipajes(int)
    * Bloquea hasta que el controlador_carga llame a liberar_robot_carga
    **/
   void carga_finalizada() {
   }

   /*
    * Libera el robot_carga para recibir otras tareas.
    * Borra todos los contenedores de la ultima carga
    **/
   bool liberar_robot_carga() {
      return true;
   }

   /*
    * Devuelve el contenedor de la pos pos.
    **/
   Contenedor get_contenedor(int pos) {
      if(pos>0)
         return Contenedor();
      return Contenedor();
   }

   /*
    * Devuelve la cantidad de contenedores cargados hasta ese momento 
    **/
   int get_cant_contenedores() {
      return 10;
   }


private:

};

#endif
