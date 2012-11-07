#ifndef API_DESPACHANTE_H_
#define API_DESPACHANTE_H_

#include "equipaje.h"

#include "cintaprincipal.h"
#include "rfid.h"

#include <set>

class ApiDespachante {
private:
	CintaPrincipal<Equipaje> cinta;
	int numero_despachante;
	Equipaje ultimo_equipaje_leido;
	bool saco_elemento;

public:
	ApiDespachante(int numero_despachante, const char* path_to_cinta_central);
	virtual ~ApiDespachante();

	Rfid leer_proximo_equipaje();
	Equipaje extraer_equipaje();
	void avanzar_cinta();
};

#endif
