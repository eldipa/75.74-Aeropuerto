#ifndef API_SCANNER_H_
#define API_SCANNER_H_

#include "equipaje.h"
#include "cintas.h"

class ApiScanner {
private:

	int numero_escaner;
	CintaCentral cinta;
public:

	ApiScanner(const char* directorio_de_trabajo, int numero_escaner);
	virtual ~ApiScanner();

	void colocar_equipaje_en_cinta_principal(const Equipaje & equipaje);

	Equipaje extraer_equipaje();
};

#endif /* APISCANNER_H_ */

