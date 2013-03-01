/*
 * mensajes.h
 *
 *  Created on: 13/11/2012
 *      Author: gonzalo
 */

#include "equipaje.h"

#ifndef MENSAJES_H_
#define MENSAJES_H_

typedef struct {
	long mtype;
	int checkin_cerrado;
	int cantidad_equipaje_total;
}MENSAJE_CHECKIN_CERRADO;

typedef struct {
	long mtype;
	int vuelo_entrante;
}MENSAJE_VUELO_ENTRANTE;

typedef struct {
	long mtype;
   int id_aeropuerto;
	int numero_de_vuelo;
	int zona_asignada;
}MENSAJE_ZONA_ASIGNADA;

typedef struct {
   long mtype;
   Equipaje equipaje_intercargo;
}MENSAJE_EQUIPAJE_INTERCARGO;

#endif /* MENSAJES_H_ */
