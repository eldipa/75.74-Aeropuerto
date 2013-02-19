#include <stdlib.h>
#include <iostream>
#include <stdio.h>
#include <unistd.h>

#include "database.h"
#include "stmt.h"
#include "tupleiter.h"

#include "constants.h"
#include "api_checkin.h"
#include "log.h"

#include <string>
#include <vector>
#include <sstream>
#include <vector>
#include "yasper.h"

void get_pasajeros(int num_vuelo, std::vector<int>& id_pasajeros);
std::vector<Equipaje>& get_equipajes(int num_vuelo, int num_pasajero, std::vector<Equipaje>& rfids);
std::string print_equipaje(const std::vector<Equipaje>& equipajes);

int main(int argc, char** argv) try {
	if (argc < 5) {
		Log::crit("Insuficientes parametros para generador_pasajeros, se esperaba (directorio_de_trabajo, config_file, num_vuelo, num_puesto_checkin)\n");
		return (1);
	}

	int num_vuelo = atoi(argv[3]);
	int num_puesto_checkin = atoi(argv[4]);
	ApiCheckIn checkin(argv[1], argv[2], num_puesto_checkin);
   std::vector<int> id_pasajeros;

   get_pasajeros(num_vuelo, id_pasajeros);

	Log::info("total_pasajeros: %d", id_pasajeros.size());

   std::vector<int>::iterator it;
   for( it=id_pasajeros.begin(); it != id_pasajeros.end(); it++) {
      std::vector<Equipaje> equipajes;
		checkin.llego_pasajero_para_checkin((*it), get_equipajes(num_vuelo, (*it), equipajes));
		Log::info("generando pasajero-checkin con equipaje: %s", print_equipaje(equipajes).c_str());
		sleep(rand()%SLEEP_LLEGADA_PASAJEROS_A_CHECKIN);
   }

	Log::info("ya llegaron todos los pasajeros del vuelo %d",  num_vuelo);
	return 0;

} catch(const std::exception &e) {
   Log::crit("%s", e.what());
} catch(...) {
   Log::crit("Critical error. Unknow exception at the end of the 'main' function.");
}

void get_pasajeros(int num_vuelo, std::vector<int>& id_pasajeros) {
	// Database db("aeropuerto", true);
   // yasper::ptr<Statement> query = db.statement("select id from Pasajero where vuelo = :vuelo");
	// query->set(":vuelo", num_vuelo);

	// yasper::ptr<TupleIterator> p_it = query->begin();
	// yasper::ptr<TupleIterator> p_end = query->end();

	// for (; (*p_it) != (*p_end); ++(*p_it)) {
	// 	id_pasajeros.push_back( p_it->at<int>(0));
	// }

   char primera_linea[255];
	FILE * file_pasajeros;
   int id_pasajero;
   int num_vuelo_asignado;

	file_pasajeros = fopen("./entrada/pasajeros.csv", "rt");
	fscanf(file_pasajeros, "%[^\n]s\n", primera_linea);

	while (fscanf(file_pasajeros, "%d:%d\n",&id_pasajero, &num_vuelo_asignado) != EOF) {
      if(num_vuelo_asignado == num_vuelo) {
         Log::debug("push pasajero %d para vuelo %d", id_pasajero, num_vuelo);
         id_pasajeros.insert(id_pasajeros.begin(), id_pasajero);
      }
	}

	fclose(file_pasajeros);
   
}

std::vector<Equipaje>& get_equipajes(int num_vuelo, int num_pasajero, std::vector<Equipaje>& equipajes) {
	// Database db("aeropuerto", true);
	// yasper::ptr<Statement> query = db.statement("select rfid from Equipaje where vuelo = :vuelo and id_pasajero = :pasajero");
	// query->set(":vuelo", num_vuelo);
	// query->set(":pasajero", num_pasajero);

	// yasper::ptr<TupleIterator> p_it = query->begin();
	// yasper::ptr<TupleIterator> p_end = query->end();

	// for (; (*p_it) != (*p_end); ++(*p_it)) {
	// 	equipajes.push_back( Equipaje( Rfid(p_it->at<int>(0), num_vuelo), rand()%TOPE_PESO_VALIJA) );
	// }
   // return equipajes;

   char primera_linea[255];
	FILE * file_equipajes;
   int pasajero;
   int vuelo;
   int rfid;

	file_equipajes = fopen("./entrada/equipajes.csv", "rt");
	fscanf(file_equipajes, "%[^\n]s\n", primera_linea);

	while (fscanf(file_equipajes, "%d:%d:%d\n",&rfid, &pasajero, &vuelo) != EOF) {
      if((vuelo ==num_vuelo) && (pasajero ==num_pasajero)) {
         Log::debug("push equipaje %d para pasajero %d, vuelo %d", rfid, pasajero, vuelo);
         equipajes.push_back( Equipaje( Rfid(rfid, vuelo), rand()%TOPE_PESO_VALIJA) );
      }
	}

	fclose(file_equipajes);
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
