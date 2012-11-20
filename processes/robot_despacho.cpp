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
#include "yasper.h"

class VueloNoAsignadoException {
public:
   VueloNoAsignadoException() {
   }
};

int get_numero_zona(ApiDespachante& api_despacho, int num_vuelo);

int main(int argc, char** argv) try {
	char path_cinta_central[300];
	char path_cinta_contenedor[300];

	if (argc < 4) {
		Log::info(
				"insuf. param para robot de despacho,se esperaba(id, zona_desde, zona_hasta...)\n");
		exit(1);
	}

	int id_robot = atoi(argv[1]);

	if (id_robot < 1) {
		Log::crit("ID de robot_despacho incorrecto %d\n", id_robot);
		exit(1);
	}

	int zona_desde = atoi(argv[2]);
	int zona_hasta = atoi(argv[3]);
	strcpy(path_cinta_contenedor, std::string(PATH_KEYS).append(PATH_CINTA_CONTENEDOR).c_str());

	ApiDespachante despachante_cinta_central(id_robot, PATH_KEYS);

	Log::info("Iniciando robot despacho(%s), cinta_central cinta_contenedor:%s\n", argv[1], path_cinta_contenedor);

	for (;;) {

		sleep(rand() % SLEEP_ROBOT_DESPACHO);

		Log::info("Robot de despacho(%s) Intentando tomar un nuevo equipaje de cinta central(%s)\n",
	             argv[1], argv[3]);

		Rfid rfid_equipaje = despachante_cinta_central.leer_proximo_equipaje();

      try {
         int num_zona = get_numero_zona(despachante_cinta_central, rfid_equipaje.numero_de_vuelo_destino);

         Log::info("Robot despacho(%s), toma el equipaje %d con destino a zona (%d)",
                   argv[1], rfid_equipaje.rfid,  num_zona);
         Equipaje equipaje = despachante_cinta_central.extraer_equipaje();
            
         if (equipaje.es_sospechoso()) {
               
            Log::info("Robot de despacho(%s) Equipaje %s sospechoso, vuelvo a ponerlo en la cinta central\n",
                      argv[1], equipaje.toString().c_str());
				despachante_cinta_central.avanzar_cinta();
         } else if ((num_zona < zona_desde) || (num_zona > zona_hasta)) {
               
            Log::info("Robot de despacho(%s) Equipaje %s no pertenece a mis zonas, vuelve a cinta central\n",
                      argv[1], equipaje.toString().c_str());
            despachante_cinta_central.avanzar_cinta();
               
         } else {

            Log::info("Robot de despacho(%s) Equipaje %s limpio envio a robot_carga zona %d\n",
                      argv[1], equipaje.toString().c_str(), num_zona);
            CintaContenedor cinta_contenedor(path_cinta_contenedor, num_zona);
            cinta_contenedor.poner_equipaje(equipaje, id_robot);
            despachante_cinta_central.avanzar_cinta();
         }
      } catch(VueloNoAsignadoException ex) {
         Log::info("Robot de despacho(%s) Equipaje %d no esta registrado o su vuelo no tiene zona asignada\n",
                   argv[1], rfid_equipaje.rfid);
         despachante_cinta_central.avanzar_cinta();
      }
   }

} catch(const std::exception &e) {
   Log::crit("%s", e.what());
} catch(...) {
   Log::crit("Critical error. Unknow exception at the end of the 'main' function.");
}

int get_numero_zona(ApiDespachante& api_despacho, int num_vuelo) {
   int zona = api_despacho.get_zona(num_vuelo);
   if(zona == -1)
      throw VueloNoAsignadoException();
   return zona;
}
