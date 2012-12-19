#ifndef CONTENEDOR_H_
#define CONTENEDOR_H_

#include "equipaje.h"
#include <vector>

#define MAX_PESO_CONTENEDOR 3000
#define MAX_VALIJAS_POR_CONTENEDOR 30
#define MAX_CONTENEDORES_POR_TRACTOR 2

class Contenedor {
public:
	Contenedor();
	void agregar_equipaje(const Equipaje& e);
	bool hay_lugar(const Equipaje& e);
   int get_equipajes() const;
	std::vector<int> get_rfids();
	virtual ~Contenedor();

private:
	int peso;
	int cant_equipajes;
	Equipaje equipajes[MAX_VALIJAS_POR_CONTENEDOR];

};

typedef struct {
	long mtype;
	int cantidad_total_contenedores;
	int vuelo_destino;
	int contenedores_actual;
	Contenedor contenedores[MAX_CONTENEDORES_POR_TRACTOR];
} BloqueContenedores; // para ponerlo en la cola de mensajes

typedef struct {
	long mtype;
	int cantidad_total_contenedores;
	Contenedor contenedor;
} ContenedorParaAvion; // para ponerlo en la cola de mensajes

#endif /* EQUIPAJE_H_ */
