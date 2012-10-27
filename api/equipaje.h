#ifndef EQUIPAJE_H_
#define EQUIPAJE_H_

#include "rfid.h"
/*
 * Es una Valija.
 * Tiene un rfid que la identifica
 **/
class Equipaje {
public:
	Equipaje(int id) : rfid(id) {}

	Rfid getRfid() { return rfid; }
private:	
   Rfid rfid;
};

#endif /* EQUIPAJE_H_ */
