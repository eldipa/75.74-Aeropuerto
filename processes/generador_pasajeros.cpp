#include <stdlib.h>

#include "database.h"
#include "stmt.h"
#include "tupleiter.h"

#include "constants.h"
#include "api_checkin.h"
#include "log.h"

int main(int argc, char** argv) {
   if (argc < 5) {
      Log::crit("Insuficientes parametros para generador_pasajeros, se esperaba (num_vuelo, id_checkin, path_to_locks, id_cinta_checkin)\n");
      return (1);
   }

   Database db("aeropuerto", true);
   ApiCheckIn checkin(atoi(argv[2]), argv[3], atoi(argv[4]));

   Log::info("GeneradorPasajeros generando pasajeros para vuelo %d, puesto_checkin %d", atoi(argv[1]), atoi(argv[2]));

   // hago un checkin por cada equipaje.En realidad hay que juntar todos los equiopajes del mismo pasajero
	std::auto_ptr<Statement> query = db.statement("select rfid from Equipaje where vuelo = :vuelo");
	query->set(":vuelo", atoi(argv[2]));

	std::auto_ptr<TupleIterator> p_it = query->begin();
	std::auto_ptr<TupleIterator> p_end = query->end();
	//Estas dos lineas no son mas que unos alias
	TupleIterator &it = *p_it;
	TupleIterator &end = *p_end;


   for(; it != end ; ++it) {
      std::vector<Equipaje> equipajes;
      Equipaje e( it.at<int>(0), rand()%TOPE_PESO_VALIJA );
      e.set_sospechoso( (rand()%5)==0 );
      equipajes.push_back(e);
      checkin.llego_pasajero_para_checkin(999, equipajes);
      Log::info("GeneradorPasajeros generando pasajero-checkin con equipaje %d", e.getRfid().rfid);
   }

   Log::info("GeneradorPasajeros ya llegaron todos los pasajeros del vuelo %d", atoi(argv[1]));
   return 0;
}
