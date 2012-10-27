#include "contenedor.h"

Contenedor::Contenedor( int num_contenedor, int peso_maximo ) :
   num_contenedor(num_contenedor),
   peso_maximo(peso_maximo), peso(0) {}

Contenedor::~Contenedor() { }

bool Contenedor::espacio_disponible(Equipaje&) { return true; }
void Contenedor::agregar_equipaje(Equipaje&) { }
