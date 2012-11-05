#include "api_torre_de_control.h"
#include "api_constants.h"

#include "database.h"
#include "stmt.h"
#include "tupleiter.h"

#include "semaphoreset.h"
#include "mutex.h"
#include "process.h"

#include "log.h"

#include <stdexcept>

void lanzar_notificadores_de_vuelos_de_intercargo(char*, int);

void ApiTorreDeControl::notificar_llegada_vuelo(int numero_vuelo) {
    SemaphoreSet control(path_torre_control, MTX_CENTRAL, CANT_MUTEX_CENTRAL);
    Mutex infoVuelos(control, MUTEX_INFO_VUELOS);
    infoVuelos.lock();

    lanzar_notificadores_de_vuelos_de_intercargo(path_torre_control, numero_vuelo);

    infoVuelos.unlock();
}

void lanzar_notificadores_de_vuelos_de_intercargo(char* path_torre_control, int numero_vuelo) {
  Database db("aeropuerto", true);

  std::auto_ptr<Statement> query = db.statement("select vuelo_destino from VuelosIntercargo where vuelo_origen = :vuelo");
  query->set(":vuelo", numero_vuelo);

  std::auto_ptr<TupleIterator> p_it = query->begin();
  std::auto_ptr<TupleIterator> p_end = query->end();

  //Estas dos lineas no son mas que unos alias
  TupleIterator &it = *p_it;
  TupleIterator &end = *p_end;

  static char vuelo_origen[10], vuelo_destino[10];
  static char *args_notificador[5] = {
        (char*)"notificador_descarga",
        path_torre_control,
        vuelo_origen,
        vuelo_destino, // Vuelo destino
        NULL
  };

  snprintf(vuelo_origen, 10, "%i", numero_vuelo);
  for(; it != end ; ++it) {
    snprintf(vuelo_destino, 10, "%i", it.at<int>(0));
    Log::info("lanzando notificador %s - %s - %s", args_notificador[1], args_notificador[2], args_notificador[3]);
    Process notificador("notificador_descarga", args_notificador);
  };

}