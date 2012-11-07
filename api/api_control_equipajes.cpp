/*
 * ApiControlEquipajes.cpp
 *
 *  Created on: 07/11/2012
 *      Author: gonzalo
 */

#include "api_control_equipajes.h"

ApiControlEquipajes::ApiControlEquipajes(unsigned int numero_productor,
		unsigned int numero_consumidor, const char* path_to_cinta_central) :
		cinta_para_extraer(path_to_cinta_central, numero_consumidor, false), cinta_para_colocar(
				path_to_cinta_central, numero_productor, false) {
}

ApiControlEquipajes::~ApiControlEquipajes() {
	// TODO Auto-generated destructor stub
}

Equipaje ApiControlEquipajes::obtener_proximo_equipaje_sospechoso() {
	Equipaje e;

	cinta_para_extraer.leer_elemento(&e);
	cinta_para_extraer.extraer_elemento();

	return e;
}

void ApiControlEquipajes::volver_a_colocar_equipaje_en_cinta_principal(const Equipaje & e) {
	cinta_para_colocar.colocar_elemento(&e);
}

void ApiControlEquipajes::avisar_equipaje_sospechoso(Rfid rfid){
	// TODO Avisar torre de control
	int a = rfid.numero_de_vuelo_destino;
	if(a==0){
		// COMPILA Y NO JODAS CON VARIABLES SIN USAR
	}
}
