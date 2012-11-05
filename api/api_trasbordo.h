#ifndef APITRASBORDO_H_
#define APITRASBORDO_H_

#include "equipaje.h"
#include "vuelo.h"
#include "cintas.h"

typedef struct {
    Vuelo vuelo_origen;
    Vuelo vuelo_destino;
} VueloTrasbordo;

class ApiTrasbordo {
public:
    VueloTrasbordo proximo_vuelo_trasbordo();
    void poner_en_cinta_principal(const Equipaje& equipaje);

    ApiTrasbordo(int id_robot, const char* path_to_trasbordo, const char* path_to_cinta_central, int id_cinta_central);
    virtual ~ApiTrasbordo();

private:

    int id_robot;
    const char* path_to_trasbordo;
    CintaCentral cinta;

};

#endif