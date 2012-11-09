#ifndef API_ADMINCONTENEDORES_H_
#define API_ADMINCONTENEDORES_H_

#include "semaphoreset.h"
#include "sharedobject.h"

class ApiAdminContenedores {
public:
   ApiAdminContenedores(const char* admin_contenedores, int cant_contenedores_inicial);
   ApiAdminContenedores(const char* admin_contenedores);

   void pedir_contenedor();
   void liberar_contenedor();

private:
   int cant_contenedores;
   std::string path;
   //SharedObject<int> cant_contenedores;
   //SemaphreSet sem;
};

#endif
