#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <cstdio>

#include "api_despachante.h"
#include "equipaje.h"
#include "constants.h"
#include "cintas.h"

#include "database.h"
#include "stmt.h"
#include "tupleiter.h"
#include "log.h"

#include <string>


int get_numero_vuelo(Rfid);
int get_numero_zona(int num_vuelo);


int main(int argc, char** argv) {

	if ((argc < 5) || ((argc - 4) % 2 != 0)) {
		Log::crit("insuf. param para robot de despacho,se esperaba(id, path_cinta_central id_cinta_central path_cinta_contenedor id_cinta_contenedor...)\n");
		exit(1);
	}

	ApiDespachante despachante_cinta_central(atoi(argv[1]), argv[2], atoi(argv[3]));
	CintaContenedor cinta_contenedor(argv[4], atoi(argv[5]));

	Log::info("Iniciando robot despacho(%s), cinta_central:%s cinta_contenedor:%s\n", 
             argv[1], argv[3], argv[5]);
   
	for (;;) {
		sleep(rand() % SLEEP_ROBOT_DESPACHO);

		Log::info("Robot de despacho(%s) Intentando tomar un nuevo equipaje de cinta central(%s)\n", argv[1], argv[3]);
     
		Rfid rfid_equipaje = despachante_cinta_central.leer_proximo_equipaje();

		int num_vuelo = get_numero_vuelo(rfid_equipaje);
		int num_zona = get_numero_zona(num_vuelo);
		Log::info("Robot despacho(%s), toma el equipaje %d con destino al vuelo %d (zona %d)",
				argv[1], rfid_equipaje.rfid, num_vuelo, num_zona);
      Equipaje equipaje = despachante_cinta_central.extraer_equipaje();
         
      if( equipaje.es_sospechoso() ) {
         Log::info("Robot de despacho(%s) Equipaje %s sospechoso, vuelvo a ponerlo en la cinta central\n", argv[1], equipaje.toString().c_str());
         //despachante_cinta_central.mover_valija(atoi(argv[1]), atoi(argv[1])+1);
         despachante_cinta_central.avanzar_cinta();
      } else {
         Log::info("Robot de despacho(%s) Equipaje %s limpio envio a robot_carga\n", argv[1], equipaje.toString().c_str());
         cinta_contenedor.poner_equipaje(equipaje);
         despachante_cinta_central.avanzar_cinta();
         //despachante_cinta_central.listo_para_recibir_valija_para(atoi(argv[1]));
      }
	}
}


int get_numero_vuelo(Rfid rfid) {
  Database db("aeropuerto", true);
  int num_vuelo = -1;

  std::auto_ptr<Statement> query = db.statement("select rfid from Equipaje where rfid = :rfid");
  query->set(":rfid", rfid.rfid);
   
  std::auto_ptr<TupleIterator> p_it = query->begin();
  std::auto_ptr<TupleIterator> p_end = query->end();

  //Estas dos lineas no son mas que unos alias
  TupleIterator &it = *p_it;
  TupleIterator &end = *p_end;

  if(it != end) {
     num_vuelo = it.at<int>(0);
  }

  return num_vuelo;
}

int get_numero_zona(int num_vuelo) {
  Database db("aeropuerto", true);
  int num_zona = -1;

  std::auto_ptr<Statement> query = db.statement("select num_zona from ZonasUtilizadas where vuelo = :vuelo");
  query->set(":vuelo", num_vuelo);
   
  std::auto_ptr<TupleIterator> p_it = query->begin();
  std::auto_ptr<TupleIterator> p_end = query->end();

  //Estas dos lineas no son mas que unos alias
  TupleIterator &it = *p_it;
  TupleIterator &end = *p_end;


  if(it != end) {
     num_zona = it.at<int>(0);
  }

  return num_zona;
}
