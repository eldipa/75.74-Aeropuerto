#ifndef API_SCANNER_H_
#define API_SCANNER_H_

#include "equipaje.h"
#include "cintas.h"
#include "messagequeue.h"

class ApiScanner {
private:

	int numero_escaner;
	MessageQueue cola_grupo_envio;
	MessageQueue cola_grupo_recepcion;

public:

	ApiScanner(const char * directorio_de_trabajo, const char * nombre_aeropuerto, int numero_de_sitio,
			int numero_escaner);
	virtual ~ApiScanner();

	void colocar_equipaje_en_cinta_principal(const Equipaje & equipaje);

	Equipaje extraer_equipaje_de_cinta_escaner();
};

#endif /* APISCANNER_H_ */

