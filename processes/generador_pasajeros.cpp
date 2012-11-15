#include <stdlib.h>

#include "database.h"
#include "stmt.h"
#include "tupleiter.h"

#include "constants.h"
#include "api_checkin.h"
#include "log.h"


int main(int argc, char** argv) {
	int num_vuelo;
	int id_checkin;
	int id_cinta_checkin;
	if (argc < 4) {
		Log::crit(
				"Insuficientes parametros para generador_pasajeros, se esperaba (num_vuelo, id_checkin, id_cinta_checkin)\n");
		return (1);
	}

	num_vuelo = atoi(argv[1]);
	id_checkin = atoi(argv[2]);
	id_cinta_checkin = atoi(argv[3]);

	Database db("aeropuerto", true);

	ApiCheckIn checkin(id_checkin, PATH_KEYS, id_cinta_checkin);

	Log::info("GeneradorPasajeros generando pasajeros para vuelo %d, puesto_checkin %d", num_vuelo,
			id_checkin);

	// hago un checkin por cada equipaje.En realidad hay que juntar todos los equiopajes del mismo pasajero
	std::auto_ptr<Statement> query = db.statement(
			"select rfid,id_pasajero from Equipaje where vuelo = :vuelo");
	query->set(":vuelo", atoi(argv[1]));

	std::auto_ptr<TupleIterator> p_it = query->begin();
	std::auto_ptr<TupleIterator> p_end = query->end();
	//Estas dos lineas no son mas que unos alias
	TupleIterator &it = *p_it;
	TupleIterator &end = *p_end;


	for (; it != end; ++it) {
		std::vector<Equipaje> equipajes;
		Equipaje e(it.at<int>(0), rand() % TOPE_PESO_VALIJA);
		e.set_sospechoso((rand() % 5) == 0);
		e.getRfid().numero_de_vuelo_destino = num_vuelo;
		equipajes.push_back(e);
		checkin.llego_pasajero_para_checkin(it.at<int>(1), equipajes);
		Log::info("GeneradorPasajeros generando pasajero-checkin con equipaje %d",
				e.getRfid().rfid);
		sleep(SLEEP_LLEGADA_PASAJEROS_A_CHECKIN);
	}

	Log::info("GeneradorPasajeros ya llegaron todos los pasajeros del vuelo %d", atoi(argv[1]));
	return 0;

}
