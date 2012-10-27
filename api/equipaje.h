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

   Equipaje(int id, int peso = 0) : rfid(id), _peso(peso) {}
   Equipaje(const Equipaje &e) : rfid(e.rfid.rfid) {
   } 

   Rfid getRfid() { return rfid; }

   std::string toString() const {
      std::ostringstream ostream;
      ostream << "Equipaje (" << rfid.rfid << ")";
      return ostream.str();
   }

   int peso() {
      return _peso;
   }

private: 
   Rfid rfid;
   int _peso;
};

#endif /* EQUIPAJE_H_ */
