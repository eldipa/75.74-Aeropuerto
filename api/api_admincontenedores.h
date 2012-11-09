#ifndef API_ADMINCONTENEDORES_H_
#define API_ADMINCONTENEDORES_H_

#include "semaphoreset.h"
#include "sharedobject.h"


class ApiAdminContenedores {
public:
   /*
    * Administra los contenedores disponibles en el aeropuerto.
    * El primer constructor se llama una sola vez.cant_contenedores_inicial recibe
    * la cantidad de contenedores con que arranca la app.
    * Los contenedores no se identifican.Solo importa la cantidad disponible en cada momento.
    **/
   ApiAdminContenedores(const char* admin_contenedores, int cant_contenedores_inicial);
   ApiAdminContenedores(const char* admin_contenedores);

   /*
    * Bloquea si no hay contenedores_disponibles.
    * Toma un contenedor en caso de que haya contenedores disponbles o se libere alguno.
    **/
   void pedir_contenedor();

   /*
    * Libera un contenedor para que pueda ser reutilizado.
    **/
   void liberar_contenedor();

private:
   int cant_contenedores;
   std::string path;
   //SharedObject<int> cant_contenedores;
   //SemaphreSet sem;
};

#endif
