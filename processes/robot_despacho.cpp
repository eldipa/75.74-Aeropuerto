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

/*
 * Devuelve el n° de vuelo del equipaje registrado en la bd.
 * devuelve (-1) si el equipaje no esta registrado.
 **/
int get_numero_vuelo(Rfid);

/*
 * Devuelve el n° de zona donde utilizado por el vuelo.
 * Consulta en la BD.
 * devuelve (-1) si el vuelo no esta en ninguna zona.
 **/

int get_numero_zona(int num_vuelo);

int main(int argc, char** argv) {
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
	strcpy(path_cinta_central, std::string(PATH_KEYS).append(PATH_CINTA_CENTRAL).c_str());
	strcpy(path_cinta_contenedor, std::string(PATH_KEYS).append(PATH_CINTA_CONTENEDOR).c_str());

	ApiDespachante despachante_cinta_central(id_robot, path_cinta_central);

	Log::info("Iniciando robot despacho(%s), cinta_central:%s cinta_contenedor:%s\n", argv[1],
			path_cinta_central, path_cinta_contenedor);

	for (;;) {
		sleep(rand() % SLEEP_ROBOT_DESPACHO);

		Log::info("Robot de despacho(%s) Intentando tomar un nuevo equipaje de cinta central(%s)\n",
				argv[1], argv[3]);

		Rfid rfid_equipaje = despachante_cinta_central.leer_proximo_equipaje();
		int num_vuelo = get_numero_vuelo(rfid_equipaje);
		int num_zona = get_numero_zona(num_vuelo);

		if ((num_zona == -1) || (num_vuelo == -1)) {
			Log::info(
					"Robot de despacho(%s) Equipaje %d no esta registrado o su vuelo no tiene zona asignada\n",
					argv[1], rfid_equipaje.rfid);
		} else {

			Log::info("Robot despacho(%s), toma el equipaje %d con destino al vuelo %d (zona %d)",
					argv[1], rfid_equipaje.rfid, num_vuelo, num_zona);
			Equipaje equipaje = despachante_cinta_central.extraer_equipaje();

			if (equipaje.es_sospechoso()) {

				Log::info(
						"Robot de despacho(%s) Equipaje %s sospechoso, vuelvo a ponerlo en la cinta central\n",
						argv[1], equipaje.toString().c_str());
				despachante_cinta_central.avanzar_cinta();

			} else if ((num_zona < zona_desde) || (num_zona > zona_hasta)) {

				Log::info(
						"Robot de despacho(%s) Equipaje %s no pertenece a mis zonas, vuelve a cinta central\n",
						argv[1], equipaje.toString().c_str());
				despachante_cinta_central.avanzar_cinta();

			} else {

				Log::info("Robot de despacho(%s) %s limpio envio a robot_carga zona %d\n",
						argv[1], equipaje.toString().c_str(), num_zona);
				CintaContenedor cinta_contenedor(path_cinta_contenedor, num_zona);
				cinta_contenedor.poner_equipaje(equipaje, id_robot);
				despachante_cinta_central.avanzar_cinta();

			}
		}
	}

}

int get_numero_vuelo(Rfid rfid) {
	Database db("aeropuerto", true);
	int num_vuelo = -1;

	yasper::ptr<Statement> query = db.statement("select vuelo from Equipaje where rfid = :rfid");
	query->set(":rfid", rfid.rfid);

	yasper::ptr<TupleIterator> p_it = query->begin();
	yasper::ptr<TupleIterator> p_end = query->end();

	//Estas dos lineas no son mas que unos alias
	TupleIterator &it = *p_it;
	TupleIterator &end = *p_end;

	if (it != end) {
		num_vuelo = it.at<int>(0);
	} else {
		Log::info("Equipaje %d, no registrado en la BD", rfid.rfid);
	}

	return num_vuelo;
}

int get_numero_zona(int num_vuelo) {
	Database db("aeropuerto", true);
	int num_zona = -1;

	yasper::ptr<Statement> query = db.statement(
			"select num_zona from ZonasUtilizadas where vuelo = :vuelo");
	query->set(":vuelo", num_vuelo);

	yasper::ptr<TupleIterator> p_it = query->begin();
	yasper::ptr<TupleIterator> p_end = query->end();

	//Estas dos lineas no son mas que unos alias
	TupleIterator &it = *p_it;
	TupleIterator &end = *p_end;

	if (it != end) {
		num_zona = it.at<int>(0);
	} else {
		Log::info("Vuelo %d, no tiene asignado ninguna zona en la BD", num_vuelo);
	}

	return num_zona;
}
