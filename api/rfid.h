#ifndef RFID_H_
#define RFID_H_

#include <string>
#include <string.h>
#include "log.h"

#define MAX_SIZE_ESCALA 30
#define MAX_CANT_ESCALAS 5

class Rfid {
public:
	Rfid(int rfid, int num_vuelo) :
      rfid(rfid), numero_de_vuelo_destino(num_vuelo) {
		memset(escala_destino, 0x00, MAX_SIZE_ESCALA);
	}
	virtual ~Rfid() {
	}

	void set_escala(const std::string& escala) {
		//Log::info("seteando escala %s", escala.c_str());
		strncpy(escala_destino, escala.c_str(), MAX_SIZE_ESCALA);
	}

	std::string get_escala() const {
		return std::string(escala_destino);
	}

// private:
	int rfid;
	int numero_de_vuelo_destino;
	int escalas[MAX_CANT_ESCALAS];
	bool sospechoso;
	char escala_destino[MAX_SIZE_ESCALA];
};

#endif /* RFID_H_ */
