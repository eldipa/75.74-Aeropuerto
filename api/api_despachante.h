#ifndef API_DESPACHANTE_H_
#define API_DESPACHANTE_H_

#include "equipaje.h"

#include "cintaprincipal.h"
#include "rfid.h"

#include <set>

class ApiDespachante {
private:
	CintaPrincipal<Equipaje> cinta;
	bool terminar;
	unsigned int numero_despachante;
public:
	ApiDespachante(unsigned int numero_despachante, const char* path_to_cinta_central);
	virtual ~ApiDespachante();

	Equipaje obtener_proximo_equipaje(const std::set<int> & vuelos_a_despachar);

};

#endif
