#ifndef EQUIPAJE_H_
#define EQUIPAJE_H_

#include "rfid.h"
/*
 * Es una Valija.
 * Tiene un rfid que la identifica
 **/
class Equipaje {
public:
	Equipaje(int id);

	Rfid getRfid();
   int peso();
   friend bool operator==(Contenedor& a, Contenedor& b);

private:	
   Rfid rfid;
};

#endif /* EQUIPAJE_H_ */
