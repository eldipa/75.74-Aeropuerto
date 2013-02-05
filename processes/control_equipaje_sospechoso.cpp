#include <iostream>
#include <stdlib.h>
#include <unistd.h>

#include "constants.h"
#include "log.h"
#include "equipaje.h"
#include "api_control_equipajes.h"

int main(int argc, char* argv[]) try {

	if (argc < 2) {
		Log::crit("Insuf parametros para controlador_equipajes_sospechosos,se esperaba (directorio_de_trabajo, pos_consumidor_cinta_central,pos_productor_cinta_central)\n");
		exit(1);
	}

	ApiControlEquipajes api_control_equipajes(argv[1], atoi(argv[2]), atoi(argv[3]));

	Log::info("Iniciando ControlEquipajeSospechoso (pos=%s)\n", argv[2]);

	for (;;) {
      Equipaje equipaje = api_control_equipajes.get_equipaje_a_controlar();
      Log::info("Recibo equipaje %d, limpio y vuelve a cinta central\n",
                equipaje.getRfid().rfid);

		// por ahora, limpia el equipaje y lo vuelve a poner al comienzo de la cinta central
		equipaje.set_sospechoso(false);
		api_control_equipajes.volver_a_colocar_equipaje_en_cinta_principal(equipaje);

	}

 } catch(const std::exception &e) {
   Log::crit("%s", e.what());
 } catch(...) {
   Log::crit("Critical error. Unknow exception at the end of the 'main' function.");
 }
