/*
 * ApiControlEquipajes.cpp
 *
 *  Created on: 07/11/2012
 *      Author: gonzalo
 */

#include "api_control_equipajes.h"

ApiControlEquipajes::ApiControlEquipajes(int pos_consumidor_cinta_central,
		int pos_productor_cinta_central, const char* path_to_cinta_central, int id_cinta_central) :
		pos_consumidor_cinta_central(pos_consumidor_cinta_central), pos_productor_cinta_central(
				pos_productor_cinta_central), cinta_central(path_to_cinta_central, id_cinta_central) {
}

ApiControlEquipajes::~ApiControlEquipajes() {
}

Equipaje ApiControlEquipajes::obtener_proximo_equipaje_sospechoso() {
	/* e = cinta_central.obtener_valija_para(pos_consumidor_cinta_central);
	 cinta_central.listo_para_recibir_valija_para(pos_consumidor_cinta_central);
	 return e;*/

	Equipaje e;
	cinta_central.leer_elemento(&e, this->pos_consumidor_cinta_central);
	cinta_central.extraer_elemento();
	return e;

}

void ApiControlEquipajes::volver_a_colocar_equipaje_en_cinta_principal(const Equipaje & e) {
	//cinta_central.poner_valija(e, 0);
	cinta_central.colocar_elemento(&e, this->pos_productor_cinta_central);
}

void ApiControlEquipajes::avisar_equipaje_sospechoso(Rfid rfid) {
	// TODO Avisar torre de control
	int a = rfid.numero_de_vuelo_destino;
	if (a == 0) {
		// MOLESTA CON VARIABLE SIN USAR
	}
}
