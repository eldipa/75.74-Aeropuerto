#ifndef CONTENEDOR_H_
#define CONTENEDOR_H_

#include "equipaje.h"
#include <vector>

/*
 * Faltan metodos para verificar si esta lleno, capacidad, etc...

 **/ 
class Contenedor {
public:
   Contenedor( int num_contenedor, int peso_maximo );
   bool tiene_el_equipaje( Equipaje& e );
   void agregar_equipaje( Equipaje& e );
   bool espacio_disponible( Equipaje& e );
   virtual ~Contenedor();

private:
   int num_contenedor;
   int peso_maximo;
   int peso;
   std::vector<Equipaje> equipajes;
   
};

#endif /* EQUIPAJE_H_ */
