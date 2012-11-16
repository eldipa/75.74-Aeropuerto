#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "equipaje.h"
#include "constants.h"

#include "api_checkin.h"
#include "log.h"

#include "database.h"
#include "stmt.h"
#include "tupleiter.h"

#include "process.h"

/*
 * Devuelve el num_vuelo del pasajero.
 * Lo toma de la BD.
 */
int get_vuelo(int id_pasajero);

int main(int argc, char *argv[]) {
   if (argc < 2) {
      Log::crit("Insuficientes parametros para puesto_checkin, se esperaba (id_puesto_checkin)\n");
      return (1);
   }

   ApiCheckIn checkin(atoi(argv[1]), PATH_KEYS);
   int vuelo_pasajero, id_pasajero;

   Log::info("Iniciando puesto_checkin(%s), conectado a cinta %i\n", argv[1], checkin.get_cinta_checkin() );
   Log::info("Puesto_checkin(%s), lanzando controlador_puesto_checkin\n", argv[1]  );

   char *args_controlador[] = { (char*) "controlador_puesto_checkin", (char*) argv[1], NULL };
	Process controlador_puesto_checkin("controlador_puesto_checkin", args_controlador);

   for(;;) {

      std::vector<Equipaje> equipajes;

      Log::info("PuestoCheckin(%d) Esperando nuevo pasajero...\n", atoi(argv[1]));
      checkin.recibir_pasajero_para_checkin(id_pasajero, equipajes);
      vuelo_pasajero = get_vuelo(id_pasajero);

      Log::info("PuestoCheckin(%d) Llego el pasajero %d con %d valijas para hacer checkin\n", atoi(argv[1]), id_pasajero, equipajes.size());      
      checkin.comienza_checkin_pasajero();

      try {
         
         if(checkin.get_vuelo_actual() == vuelo_pasajero) {
            Log::info("PuestoCheckin(%d) comienza checkin del pasajero %d\n", atoi(argv[1]), id_pasajero);             

            //envio los equipajes a la cinta de checkin.
            std::vector<Equipaje>::iterator it;
            for( it=equipajes.begin();it!=equipajes.end();it++ ) {
               Log::info("PuestoCheckin(%d) enviando equipaje %d a cinta de checkin\n", atoi(argv[1]), it->getRfid().rfid);             
               checkin.registrar_equipaje(*it);
            }
            
         } else {
            Log::info("PuestoCheckin(%d) el pasajero %d vino al puesto equivocado: vuelo_pasajero%d vuelo_checkin:%d\n", 
                      atoi(argv[1]), id_pasajero, vuelo_pasajero, checkin.get_vuelo_actual());            
         }
      } catch (PuestoCheckinSinVueloAsignado) {
         Log::info("PuestoCheckin(%d) No hay checkin habilitado en este momento\n", atoi(argv[1]) );
      }
      checkin.fin_checkin_pasajero();
   }
}

int get_vuelo(int id_pasajero) {
	Database db("aeropuerto", true);
	int num_vuelo = -1;

	std::auto_ptr<Statement> query = db.statement(
			"select vuelo from Pasajero where id = :id_pasajero");
	query->set(":id_pasajero", id_pasajero);

	std::auto_ptr<TupleIterator> p_it = query->begin();
	std::auto_ptr<TupleIterator> p_end = query->end();

	//Estas dos lineas no son mas que unos alias
	TupleIterator &it = *p_it;
	TupleIterator &end = *p_end;

	if (it != end) {
		num_vuelo = it.at<int>(0);
	} else {
		Log::crit("PuestoCheckin, llego un pasajero que no esta en la BD!!!");
	}

	return num_vuelo;

}
