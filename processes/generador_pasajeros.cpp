#include <stdlib.h>

#include "database.h"
#include "stmt.h"
#include "tupleiter.h"

#include "constants.h"
#include "api_checkin.h"
#include "log.h"

#include <string>
#include <vector>
#include <sstream>

#include "yasper.h"

void get_pasajeros(int num_vuelo, std::vector<int>& id_pasajeros);
std::vector<Equipaje>& get_equipajes(int num_vuelo, int num_pasajero, std::vector<Equipaje>& rfids);
std::string print_equipaje(const std::vector<Equipaje>& equipajes);

int main(int argc, char** argv) try {
	if (argc < 3) {
		Log::crit("Insuficientes parametros para generador_pasajeros, se esperaba (num_vuelo, num_puesto_checkin)\n");
		return (1);
	}

	int num_vuelo = atoi(argv[1]);
	int num_puesto_checkin = atoi(argv[2]);
	ApiCheckIn checkin(num_puesto_checkin, PATH_KEYS);
   std::vector<int> id_pasajeros;

   get_pasajeros(num_vuelo, id_pasajeros);

	Log::info("GeneradorPasajeros(vuelo=%d) total_pasajeros: %d", num_vuelo, id_pasajeros.size());

   std::vector<int>::iterator it;
   for( it=id_pasajeros.begin(); it != id_pasajeros.end(); it++) {
      std::vector<Equipaje> equipajes;
		checkin.llego_pasajero_para_checkin((*it), get_equipajes(num_vuelo, (*it), equipajes));
		Log::info("GeneradorPasajeros(vuelo=%d) generando pasajero-checkin con equipaje: %s", num_vuelo, print_equipaje(equipajes).c_str());
		sleep(SLEEP_LLEGADA_PASAJEROS_A_CHECKIN);
   }

	Log::info("GeneradorPasajeros(vuelo=%d) ya llegaron todos los pasajeros del vuelo %d", num_vuelo, num_vuelo);
	return 0;

} catch(const std::exception &e) {
   Log::crit("%s", e.what());
} catch(...) {
   Log::crit("Critical error. Unknow exception at the end of the 'main' function.");
}

void get_pasajeros(int num_vuelo, std::vector<int>& id_pasajeros) {
	Database db("aeropuerto", true);
   yasper::ptr<Statement> query = db.statement("select id from Pasajero where vuelo = :vuelo");
	query->set(":vuelo", num_vuelo);

	yasper::ptr<TupleIterator> p_it = query->begin();
	yasper::ptr<TupleIterator> p_end = query->end();

	for (; (*p_it) != (*p_end); ++(*p_it)) {
		id_pasajeros.push_back( p_it->at<int>(0));
	}
   
}

std::vector<Equipaje>& get_equipajes(int num_vuelo, int num_pasajero, std::vector<Equipaje>& equipajes) {
	Database db("aeropuerto", true);
	yasper::ptr<Statement> query = db.statement("select rfid from Equipaje where vuelo = :vuelo and id_pasajero = :pasajero");
	query->set(":vuelo", num_vuelo);
	query->set(":pasajero", num_pasajero);

	yasper::ptr<TupleIterator> p_it = query->begin();
	yasper::ptr<TupleIterator> p_end = query->end();

	for (; (*p_it) != (*p_end); ++(*p_it)) {
		equipajes.push_back( Equipaje( Rfid(p_it->at<int>(0), num_vuelo), rand()%TOPE_PESO_VALIJA) );
	}
   return equipajes;
}

std::string print_equipaje(const std::vector<Equipaje>& equipajes) {
   std::stringstream sstr;
   sstr << "[ ";

   std::vector<Equipaje>::const_iterator it;
   for(it = equipajes.begin(); it != equipajes.end();it++) {
      sstr << (int)(*it) << " , ";
   }

   sstr << "]";
   return sstr.str();
   
}
