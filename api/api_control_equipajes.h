#ifndef APICONTROLEQUIPAJES_H_
#define APICONTROLEQUIPAJES_H_

#include "equipaje.h"

#include "cintas.h"
#include "rfid.h"

class ApiControlEquipajes {
private:
	int pos_consumidor_cinta_central;
	int pos_productor_cinta_central;
	CintaCentral cinta_central;

public:
	ApiControlEquipajes(int pos_consumidor_cinta_central, int pos_productor_cinta_central, const char* path_to_cinta_central, int id_cinta_central);
	virtual ~ApiControlEquipajes();

	Equipaje obtener_proximo_equipaje_sospechoso();

	void volver_a_colocar_equipaje_en_cinta_principal(const Equipaje & e);

	void avisar_equipaje_sospechoso(Rfid rfid);
};

#endif /* APICONTROLEQUIPAJES_H_ */
