#ifndef APICONTROLEQUIPAJES_H_
#define APICONTROLEQUIPAJES_H_

#include "equipaje.h"

#include "cintaprincipal.h"
#include "rfid.h"

class ApiControlEquipajes {
private:
	CintaPrincipal<Equipaje> cinta_para_extraer;
	CintaPrincipal<Equipaje> cinta_para_colocar;

public:
	ApiControlEquipajes(unsigned int numero_productor, unsigned int numero_consumidor,
			const char* path_to_cinta_central);
	virtual ~ApiControlEquipajes();

	Equipaje obtener_proximo_equipaje_sospechoso();
	void volver_a_colocar_equipaje_en_cinta_principal(const Equipaje & e);

	void avisar_equipaje_sospechoso(Rfid rfid);
};

#endif /* APICONTROLEQUIPAJES_H_ */
