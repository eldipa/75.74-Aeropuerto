#include <iostream>
#include <stdlib.h>
#include <unistd.h>

#include <map>
#include <string>

#include "api_carga.h"
#include "contenedor.h"
#include "equipaje.h"
#include "log.h"
#include "cintas.h"
#include "api_admincontenedores.h"

int main(int argc, char** argv) {
	int numero_de_vuelo;
	Equipaje equipaje;
	bool checkin_cerro;

	if (argc < 9) {
		Log::crit(
				"Insuficientes parametros para robot de carga, se esperaba (id, path_cinta_contenedor, id_cinta_contenedor, path_controlador_carga, path_admin_contenedores, path_cola_tractores, path_semaforo_despachante_carga, path_shared_memory_despachante_carga)\n");
		exit(1);
	}

	std::map<std::string, Contenedor> contenedores_por_escala;
	//CintaContenedor cinta_contenedor(argv[2], atoi(argv[3]));
	ApiCarga api_carga(atoi(argv[1]), argv[2], atoi(argv[3]), argv[6], argv[7], argv[8]);
	ApiAdminContenedores api_admin_contenedores(argv[5]);

	int equipajes_por_cargar, equipajes_cargados;
	equipajes_por_cargar = equipajes_cargados = 0;

	Log::info("Iniciando robot carga(%s), %s\n", argv[1], argv[2]);

	for (;;) {
		checkin_cerro = false;
		equipajes_cargados = 0;

		Log::info("RobotCarga(%s) Bloqueo esperando orden del controlador de carga\n", argv[1]);
		//equipajes_por_cargar = api_carga.get_equipajes_por_cargar();
		equipajes_por_cargar = 10;
		Log::info("RobotCarga(%s) Llego orden de carga, cargar %d equipajes\n", argv[1],
				equipajes_por_cargar);

		while (!checkin_cerro) {
			sleep(rand() % 10);
			Log::info("RobotCarga(%s) Intentando tomar un nuevo equipaje de cinta(%s)\n", argv[1],
					argv[2]);
			equipaje = api_carga.sacar_equipaje();
			if (api_carga.checkin_cerrado()) {
				checkin_cerro = true;
				break;
			}
			equipajes_cargados++;
			numero_de_vuelo = equipaje.getRfid().numero_de_vuelo_destino;

		}

		equipajes_por_cargar = api_carga.obtener_cantidad_equipaje_total();

		while (equipajes_cargados < equipajes_por_cargar) {

			Equipaje equipaje = api_carga.sacar_equipaje();

			Log::info("RobotCarga(%s) se tomo el equipaje %s con escala destino '%s' peso=%d\n",
					argv[1], equipaje.toString().c_str(), equipaje.getRfid().get_escala().c_str(),
					equipaje.peso());

			numero_de_vuelo = equipaje.getRfid().numero_de_vuelo_destino;

			if (equipaje.peso() <= MAX_PESO_CONTENEDOR) {
				std::string escala = equipaje.getRfid().get_escala();

				if (contenedores_por_escala.find(escala) == contenedores_por_escala.end()) {
					Log::crit(
							"RobotCarga(%s) no tengo contenedor, pido contenedor para escala '%s'\n",
							argv[1], escala.c_str());
					api_admin_contenedores.pedir_contenedor();
					contenedores_por_escala.insert(
							std::pair<std::string, Contenedor>(escala, Contenedor()));
				}

				if (!contenedores_por_escala[escala].hay_lugar(equipaje)) {
					Log::crit("RobotCarga(%s) contenedor lleno, pido contenedor para escala '%s'\n",
							argv[1], escala.c_str());
					api_carga.agregar_contenedor_cargado(contenedores_por_escala[escala]);
					api_admin_contenedores.pedir_contenedor();
					contenedores_por_escala[escala] = Contenedor();
				}
				contenedores_por_escala[escala].agregar_equipaje(equipaje);
				equipajes_cargados++;

				Log::crit(
						"RobotCarga(%s) pongo equipaje %s en contenedor de escala '%s'.ya carge %d/%d equipajes\n",
						argv[1], equipaje.toString().c_str(), escala.c_str(), equipajes_cargados,
						equipajes_por_cargar);

			} else {
				Log::crit(
						"RobotCarga(%s) El equipaje %s es mas grande que el propio contenedor!!!\n",
						argv[1], equipaje.toString().c_str());
				equipajes_cargados++;
			}
		}

		// cargo el resto de los contenedores
		std::map<std::string, Contenedor>::iterator it;
		for (it = contenedores_por_escala.begin(); it != contenedores_por_escala.end(); it++)
			api_carga.agregar_contenedor_cargado((*it).second);

		Log::info("Carga finalizada, enviando contenedores a tractores");
		api_carga.enviar_contenedores_a_avion(numero_de_vuelo);
		contenedores_por_escala.clear();

		Log::info("RobotCarga(%s) fin de carga de equipajes del vuelo\n", argv[1]);
	}

}
