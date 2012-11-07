#ifndef API_SCANNER_H_
#define API_SCANNER_H_

#include "equipaje.h"

#include "cintaprincipal.h"

class ApiScanner {
private:

	CintaPrincipal<Equipaje> cinta;
	unsigned int numero_escaner;
public:

	ApiScanner(unsigned int numero_escaner, const char* path_to_cinta_central);
	virtual ~ApiScanner();

	void colocar_equipaje_en_cinta_principal(const Equipaje & equipaje);
};


#endif /* APISCANNER_H_ */


