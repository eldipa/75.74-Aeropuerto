#ifndef API_DESPACHANTE_H_
#define API_DESPACHANTE_H_

#include "api_constants.h"
#include "equipaje.h"
#include "cintas.h"
#include "rfid.h"

#include <stdexcept>

#include "torre_de_control.h"

class ApiDespachante {
private:

	CintaCentral cinta;
	int numero_despachante;

	Equipaje ultimo_equipaje_leido;
	bool saco_elemento;

   CLIENT *clnt_torre_de_control;

public:
	ApiDespachante(const char* directorio_de_trabajo, const char* config_file,const char * nombre_app, int numero_despachante);
	virtual ~ApiDespachante();

	Rfid leer_proximo_equipaje();
	Equipaje extraer_equipaje();
	void avanzar_cinta();

   /*
    * Se comunica con la torre de control via ipc.
    **/
   void asignar_vuelo(int zona, int vuelo);
   void desasignar_vuelo(int zona);
   int get_zona(int num_vuelo);
   
   static const int cant_ipcs = 2;
};

#endif
