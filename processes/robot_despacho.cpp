#include <iostream>
#include <stdlib.h>
#include <unistd.h>

#include "api_zona.h"
#include "api_admin.h"
#include "api_despachante.h"
#include "api_scanner.h"
#include "log.h"

#include "contenedor.h"
#include "equipaje.h"
#include "constants.h"

#include <map>
#include <string>
#include "cintas.h"

/*
 * Argumentos para el robot de despacho.
 * ./robot_despacho <id_robot> <cinta_in> <cinta_zona1> <zona1> ... <cinta_zonan> <zonan>
 * En donde la cinta i lleva equipajes hacia la zona i.
 **/

#include <set>

bool es_sospechoso(Equipaje& e) {
   return e.getRfid().sospechoso;
}

int get_numero_vuelo(Equipaje e) {
   return e.getRfid().numero_de_vuelo_destino;
}

int get_numero_zona(Equipaje e) {
   e.getRfid();
   return 0;
}

int main(int argc, char** argv) {

	if ((argc < 5) || ((argc - 4) % 2 != 0)) {
		Log::crit("insuf. param para robot de despacho,se esperaba(id, path_cinta_central id_cinta_central path_cinta_contenedor id_cinta_contenedor...)\n");
		exit(1);
	}

	//Equipaje equipaje;
	ApiDespachante despachante_cinta_central(atoi(argv[1]), argv[2], atoi(argv[3]));
	CintaContenedor cinta_contenedor(argv[4], atoi(argv[5]));

	Log::info("Iniciando robot despacho(%s), cinta_central:%s cinta_contenedor:%s\n", argv[1],
			argv[3], argv[5]);

	for (;;) {
		sleep(rand() % SLEEP_ROBOT_DESPACHO);

		Log::info("Robot de despacho(%s) Intentando tomar un nuevo equipaje de cinta central(%s)\n", argv[1], argv[3]);
     
      Equipaje equipaje = despachante_cinta_central.obtener_valija_para(atoi(argv[1]));

		int num_vuelo = get_numero_vuelo(equipaje);
		int num_zona = get_numero_zona(num_vuelo);
		Log::info("Robot despacho(%s), toma el equipaje %s con destino al vuelo %d (zona %d)",
				argv[1], equipaje.toString().c_str(), num_vuelo, num_zona);

      if( es_sospechoso(equipaje) ) {
         Log::info("Robot de despacho(%s) Equipaje %s sospechoso, vuelvo a ponerlo en la cinta central\n", argv[1], equipaje.toString().c_str());
         despachante_cinta_central.listo_para_recibir_valija_para(atoi(argv[1]));
         despachante_cinta_central.mover_valija(atoi(argv[1]), atoi(argv[1])+1);
      } else {
         cinta_contenedor.poner_equipaje(equipaje);
      }
             
      /*
		Rfid rfid_equipaje = despachante_cinta_central.leer_proximo_equipaje();
		if (rfid_equipaje.numero_de_vuelo_destino == mis_vuelos)
			equipaje = despachante_cinta_central.extraer_equipaje();

		int num_vuelo = admin.get_numero_vuelo(equipaje);
		int num_zona = admin.get_numero_zona(num_vuelo);
		Log::info("Robot despacho(%s), toma el equipaje %s con destino al vuelo %d (zona %d)",
				argv[1], equipaje.toString().c_str(), num_vuelo, num_zona);

		cinta_contenedor.poner_equipaje(equipaje);
		Log::info("Robot de despacho(%s)  pongo equipaje %s en la cinta de contenedor %s \n",
				argv[1], equipaje.toString().c_str(), argv[5]);
      */

	}

}
