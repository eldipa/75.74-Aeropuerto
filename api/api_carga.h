#ifndef API_CARGA_H_
#define API_CARGA_H_

#include "equipaje.h"
#include "contenedor.h"

#include <vector>

class ApiCarga {
private:
   std::string path;
   int id_robot_carga;

public:

   /*
    * Api que maneja la sincronizacion y el pasaje de contenedores entre el robot_carga y el controlador_de_carga.
    * El primer constructor crea los ipcs.Se llama solo una vez por cada controlador_de_carga.
    * recibe un path_carga y el id_robot_carga.Va a existir una ApiCarga por cada robot_carga.
    **/
   ApiCarga(const char* path_carga, int id_robot_carga, bool create):path(path_carga),id_robot_carga(id_robot_carga) {
      create = !create;
   }

   ApiCarga(const char* path_carga, int id_robot_carga):path(path_carga),id_robot_carga(id_robot_carga) {
   }

   /*
    * Bloquea hasta que el robot carga cargue n equipaejes en contenedores.
    * No bloquea y retorna un vector vacío si el robot_carga esta ocupado en otra carga.
    * En caso contrario devulve los contenedores con equipajes.
    * Cada contenedor guarda los rfids
    **/
   std::vector<Contenedor> cargar_equipajes(int n) {
      n = n -1;
      return std::vector<Contenedor>();
   }

   /*
    * Lo llama el robot_carga.
    * Bloquea hasta que el controlador de carga llame a cargar_equipajes.
    * Devuelve la cantidad de equipajes que el robot_carga debe cargar.
    **/
   int get_equipajes_por_cargar() {
      return 10;
   }

   /*
    * Guarda el contenedor con equipajes recibidos por el robot_carga.
    * Guarda los contenedores en una sharedmemory
    **/
   void contenedor_cargado(Contenedor& c) {
      c.get_rfids();
   }



private:

};

#endif
