/*
 * ApiControlEquipajes.cpp
 *
 *  Created on: 07/11/2012
 *      Author: gonzalo
 */

#include "api_control_equipajes.h"

ApiControlEquipajes::ApiControlEquipajes(int numero_productor, int numero_consumidor,
		const char* path_to_cinta_central) :
		cinta_ppal(path_to_cinta_central, true) {
	char msgerror [1024];
	if (numero_productor < 1 || numero_consumidor < 1) {
		sprintf(msgerror, "Error ID = %d, ID = %d, invalidos \n", numero_productor,
				numero_consumidor);
		// TODO tirar ValueError
		//throw ErrorEnCintaPrincipal(msgerror);
		throw "ERROR";
	}
}

ApiControlEquipajes::~ApiControlEquipajes() {
	// TODO Auto-generated destructor stub
}

Equipaje ApiControlEquipajes::obtener_proximo_equipaje_sospechoso() {
	Equipaje e;

	cinta_ppal.leer_elemento(&e, this->numero_consumidor_cinta_ppal);
	cinta_ppal.extraer_elemento();

	return e;
}

void ApiControlEquipajes::volver_a_colocar_equipaje_en_cinta_principal(const Equipaje & e) {
	cinta_ppal.colocar_elemento(&e, this->numero_productor_cinta_ppal);
}

void ApiControlEquipajes::avisar_equipaje_sospechoso(Rfid rfid) {
	// TODO Avisar torre de control
	int a = rfid.numero_de_vuelo_destino;
	if (a == 0) {
		// MOLESTA CON VARIABLE SIN USAR
	}
}
