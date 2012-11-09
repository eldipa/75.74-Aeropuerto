#include "api_admincontenedores.h"

ApiAdminContenedores::ApiAdminContenedores(const char* path_admin_contenedores, int cant_contenedores_inicial) :
   cant_contenedores(cant_contenedores_inicial), path(path_admin_contenedores) {
}

ApiAdminContenedores::ApiAdminContenedores(const char* path_admin_contenedores) :
   path(path_admin_contenedores) {
}

void ApiAdminContenedores::pedir_contenedor() {
}

void ApiAdminContenedores::liberar_contenedor() {
}
