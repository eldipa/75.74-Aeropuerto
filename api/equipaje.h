#ifndef EQUIPAJE_H_
#define EQUIPAJE_H_

#include "rfid.h"
#include <string>
#include <sstream>
/*
 * Es una Valija.
 * Tiene un rfid que la identifica
 **/
class Equipaje {
public:

   Equipaje(int id) : rfid(id), peso(0) {}
   Equipaje(int id, int peso) : rfid(id), peso(peso) {}
   Equipaje(const Equipaje &e) : rfid(e.rfid.rfid) {
   } 

   Rfid getRfid() { return rfid; }

   std::string toString() const {
      std::ostringstream ostream;
      ostream << "Equipaje (" << rfid.rfid << ")";
      return ostream.str();
   }

   int peso() {
      return peso;
   }

private: 
   Rfid rfid;
   int peso;
};

#endif /* EQUIPAJE_H_ */
