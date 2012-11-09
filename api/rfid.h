#ifndef RFID_H_
#define RFID_H_

#include <string>

class Rfid {
public:
   Rfid( int rfid ) : rfid(rfid) {
   }
   virtual ~Rfid() {}
// private:
   int rfid;
   int numero_de_vuelo_destino;
   bool sospechoso;
   std::string escala_destino;
};

#endif /* RFID_H_ */
