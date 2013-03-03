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
#include "yasper.h"

/*
 * Devuelve el num_vuelo del pasajero.
 * Lo toma de la BD.
 */
int get_vuelo(int id_pasajero);

int main(int argc, char *argv[]) try {
   if (argc < 4) {
      Log::crit("Insuficientes parametros para puesto_checkin, se esperaba (directorio_de_trabajo, config_file, id_puesto_checkin, id_cinta_checkin)\n");
      return (1);
   }

   //DEBUG
   chdir("processes");

   ApiCheckIn checkin(argv[1], argv[2], atoi(argv[3]), atoi(argv[4]), true);
   int vuelo_pasajero, id_pasajero;

   Log::info("Iniciando puesto_checkin(%s), conectado a cinta %i\n", argv[3], checkin.get_cinta_checkin() );
   Log::info("lanzando controlador_puesto_checkin\n" );

   char *args_controlador[] = { (char*) "controlador_puesto_checkin", (char*) argv[1], (char*)argv[2],
                                (char*) argv[3], (char*)"./entrada/equipajes_intercargo.csv", NULL };
	Process controlador_puesto_checkin("controlador_puesto_checkin", args_controlador);

   for(;;) {

      std::vector<Equipaje> equipajes;

      Log::info("Esperando nuevo pasajero...\n");
      checkin.recibir_pasajero_para_checkin(id_pasajero, equipajes);
      vuelo_pasajero = get_vuelo(id_pasajero);

      Log::info("Llego el pasajero %d con %d valijas para hacer checkin\n", id_pasajero, equipajes.size());
      checkin.comienza_checkin_pasajero();

      try {
         
         if(checkin.get_vuelo_actual() == vuelo_pasajero) {
            Log::info("comienza checkin del pasajero %d\n", id_pasajero);

            //envio los equipajes a la cinta de checkin.
            std::vector<Equipaje>::iterator it;
            for( it=equipajes.begin();it!=equipajes.end();it++ ) {
               Log::info("enviando equipaje %d a cinta de checkin\n", it->getRfid().rfid);
               checkin.registrar_equipaje(*it);
            }
            
         } else {
            Log::info("el pasajero %d vino al puesto equivocado: vuelo_pasajero%d vuelo_checkin:%d\n",
                       id_pasajero, vuelo_pasajero, checkin.get_vuelo_actual());
         }
      } catch (PuestoCheckinSinVueloAsignado &) {
         Log::info("No hay checkin habilitado en este momento\n" );
      }
      checkin.fin_checkin_pasajero();
   }
} catch(const std::exception &e) {
   Log::crit("%s", e.what());
} catch(...) {
   Log::crit("Critical error. Unknow exception at the end of the 'main' function.");
}

int get_vuelo(int id_pasajero) {
	// Database db("aeropuerto", true);
	// int num_vuelo = -1;

	// yasper::ptr<Statement> query = db.statement(
	// 		"select vuelo from Pasajero where id = :id_pasajero");
	// query->set(":id_pasajero", id_pasajero);

	// yasper::ptr<TupleIterator> p_it = query->begin();
	// yasper::ptr<TupleIterator> p_end = query->end();

	// //Estas dos lineas no son mas que unos alias
	// TupleIterator &it = *p_it;
	// TupleIterator &end = *p_end;

	// if (it != end) {
	// 	num_vuelo = it.at<int>(0);
	// } else {
	// 	Log::crit("llego un pasajero que no esta en la BD!!!");
	// }


   char primera_linea[255];
	FILE * file_pasajeros;
   int num_vuelo = -1;
   int pasajero;
   int num_vuelo_asignado;

	file_pasajeros = fopen("./entrada/pasajeros.csv", "rt");
	fscanf(file_pasajeros, "%[^\n]s\n", primera_linea);

	while ((fscanf(file_pasajeros, "%d:%d\n",&pasajero, &num_vuelo_asignado) != EOF) && (num_vuelo == -1)) {
      if(id_pasajero == pasajero) {
         num_vuelo = num_vuelo_asignado;
         Log::debug("search vuelo para pasajero %d.respuesta vuelo=%d", id_pasajero, num_vuelo);
      }
	}

	fclose(file_pasajeros);

	return num_vuelo;

}
