#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "equipaje.h"
#include "constants.h"

#include "api_checkin.h"
#include "log.h"

int get_vuelo(int id_pasajero) {
   id_pasajero = id_pasajero;
   return 0;
}

int main(int argc, char *argv[]) {

   if (argc < 4) {
      Log::crit("Insuficientes parametros para puesto_checkin, se esperaba (id, path_cinta num_cinta)\n");
      return (1);
   }

   ApiCheckIn checkin(atoi(argv[1]), argv[2], atoi(argv[3]));
   int vuelo_pasajero, id_pasajero;

   Log::info("Iniciando puesto_checkin(%s), conectado a cinta %i\n", argv[1], atoi(argv[3]) );

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
