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

void get_pasajeros(int num_vuelo, std::vector<int>& id_pasajeros);
std::vector<Equipaje>& get_equipajes(int num_vuelo, int num_pasajero, std::vector<Equipaje>& rfids);
std::string print_equipaje(const std::vector<Equipaje>& equipajes);

int main(int argc, char** argv) {
	if (argc < 3) {
		Log::crit("Insuficientes parametros para generador_pasajeros, se esperaba (num_vuelo, num_puesto_checkin)\n");
		return (1);
	}
	Log::info("GeneradorPasajeros generando pasajeros para vuelo %d, puesto_checkin %d", atoi(argv[1]),atoi(argv[2]));

	int num_vuelo = atoi(argv[1]);
	int num_puesto_checkin = atoi(argv[2]);
	ApiCheckIn checkin(num_puesto_checkin, PATH_KEYS);
   std::vector<int> id_pasajeros;

   get_pasajeros(num_vuelo, id_pasajeros);

   std::vector<int>::iterator it;
   for( it=id_pasajeros.begin(); it != id_pasajeros.end(); it++) {
      std::vector<Equipaje> equipajes;
		checkin.llego_pasajero_para_checkin((*it), get_equipajes(num_vuelo, (*it), equipajes));
		Log::info("GeneradorPasajeros generando pasajero-checkin con equipaje: %s", print_equipaje(equipajes).c_str());
		sleep(SLEEP_LLEGADA_PASAJEROS_A_CHECKIN);
   }

	Log::info("GeneradorPasajeros ya llegaron todos los pasajeros del vuelo %d", atoi(argv[1]));
	return 0;

}

void get_pasajeros(int num_vuelo, std::vector<int>& id_pasajeros) {
	Database db("aeropuerto", true);
	std::auto_ptr<Statement> query = db.statement("select id from Pasajero where vuelo = :vuelo");
	query->set(":vuelo", num_vuelo);

	std::auto_ptr<TupleIterator> p_it = query->begin();
	std::auto_ptr<TupleIterator> p_end = query->end();

	for (; (*p_it) != (*p_end); ++(*p_it)) {
		id_pasajeros.push_back( p_it->at<int>(0));
	}
   
}

std::vector<Equipaje>& get_equipajes(int num_vuelo, int num_pasajero, std::vector<Equipaje>& equipajes) {
	Database db("aeropuerto", true);
	std::auto_ptr<Statement> query = db.statement("select rfid from Equipaje where vuelo = :vuelo and id_pasajero = :pasajero");
	query->set(":vuelo", num_vuelo);
	query->set(":pasajero", num_pasajero);

	std::auto_ptr<TupleIterator> p_it = query->begin();
	std::auto_ptr<TupleIterator> p_end = query->end();

	for (; (*p_it) != (*p_end); ++(*p_it)) {
		equipajes.push_back( Equipaje(p_it->at<int>(0)) );
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
