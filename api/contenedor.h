#ifndef CONTENEDOR_H_
#define CONTENEDOR_H_

#include "equipaje.h"

/*
 * Faltan metodos para verificar si esta lleno, capacidad, etc...
 **/ 
class Contenedor {
public:
	Contenedor();
	bool tiene_el_equipaje( Equipaje& );
	void agregar_equipaje( Equipaje& );
	virtual ~Contenedor();

private:
	std::vector<Equipaje> euipajes;
	
};

#endif /* EQUIPAJE_H_ */
