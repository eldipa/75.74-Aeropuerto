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
#include <cstdio>
#include <string.h>
#include "yasper.h"

void lanzar_notificadores_de_vuelos_de_intercargo(const char*, int);

ApiTorreDeControl::ApiTorreDeControl(const char* path_torre_control) :
   path_torre_control(path_torre_control),
   queue_zonas(path_torre_control, Q_ZONAS),
   queue_puestos_checkin(path_torre_control, Q_PUESTOS_CHECKIN),
   queue_contenedores(path_torre_control, Q_CONTENEDORES) {

   
}

void ApiTorreDeControl::notificar_llegada_vuelo(int numero_vuelo) {
    SemaphoreSet control(path_torre_control.c_str(), MTX_CENTRAL, CANT_MUTEX_CENTRAL);
    Mutex infoVuelos(control, MUTEX_INFO_VUELOS);
    infoVuelos.lock();

    lanzar_notificadores_de_vuelos_de_intercargo(path_torre_control.c_str(), numero_vuelo);

    infoVuelos.unlock();
}

void lanzar_notificadores_de_vuelos_de_intercargo(const char* path_torre_control, int numero_vuelo) {
  char path[300];
  strcpy(path, path_torre_control);
 
  Database db("aeropuerto", true);

  yasper::ptr<Statement> query = db.statement("select vuelo_destino from VuelosIntercargo where vuelo_origen = :vuelo");
  query->set(":vuelo", numero_vuelo);

  yasper::ptr<TupleIterator> p_it = query->begin();
  yasper::ptr<TupleIterator> p_end = query->end();

  //Estas dos lineas no son mas que unos alias
  TupleIterator &it = *p_it;
  TupleIterator &end = *p_end;

  static char vuelo_origen[10], vuelo_destino[10];
  static char *args_notificador[5] = {
        (char*)"notificador_descarga",
        path,
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

void ApiTorreDeControl::pedir_contenedor() {
   tMsgContenedores msg;
   queue_contenedores.pull(&msg, sizeof(tMsgContenedores)-sizeof(long));
}

void ApiTorreDeControl::liberar_contenedor() {
   tMsgContenedores msg;
   msg.mtype = 1;
   queue_contenedores.push(&msg, sizeof(tMsgContenedores)-sizeof(long));
}

int ApiTorreDeControl::pedir_puesto_checkin(int num_vuelo) {
   num_vuelo = num_vuelo;
   tMsgCheckin msg;
   queue_puestos_checkin.pull(&msg, sizeof(tMsgCheckin)-sizeof(long));
   return msg.puesto_checkin;
}
 
void ApiTorreDeControl::liberar_puesto_checkin(int num_puesto_checkin) {
   tMsgCheckin msg;
   msg.mtype = 1;
   msg.puesto_checkin = num_puesto_checkin;
   queue_puestos_checkin.push(&msg, sizeof(tMsgCheckin)-sizeof(long));
}

int ApiTorreDeControl::pedir_zona(int num_vuelo) {
   Log::info("ApiTorreDeControl: pidiendo zona libre");
   num_vuelo = num_vuelo;
   tMsgZona msg;
   queue_zonas.pull(&msg, sizeof(tMsgZona)-sizeof(long));
   return msg.num_zona;
}

void ApiTorreDeControl::liberar_zona(int num_zona) {
   tMsgZona msg;
   msg.mtype = 1;
   msg.num_zona = num_zona;
   queue_zonas.push(&msg, sizeof(tMsgZona)-sizeof(long));
}
