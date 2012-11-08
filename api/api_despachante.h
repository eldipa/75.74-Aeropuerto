#ifndef API_DESPACHANTE_H_
#define API_DESPACHANTE_H_

#include "equipaje.h"

#include "cintas.h"
#include "rfid.h"

#include <set>

class ApiDespachante {
private:
	CintaCentral cinta;
	int numero_despachante;
   
	// Equipaje ultimo_equipaje_leido;
	// bool saco_elemento;
   
public:
	ApiDespachante(int numero_despachante, const char* path_to_cinta_central, int id_cinta_central);
	virtual ~ApiDespachante();

	Rfid leer_proximo_equipaje();
	Equipaje extraer_equipaje();
	void avanzar_cinta();

   // interfaz para uasr la cinta de martin ahsta que la otra ande.
   const Equipaje& obtener_valija_para(int robot_id);

   void mover_valija(int desde_robot_id, int hasta_robot_id);

   void listo_para_recibir_valija_para(int robot_id);

   void poner_valija(const Equipaje& e, int robot_id);

};

#endif
