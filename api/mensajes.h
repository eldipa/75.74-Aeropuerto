/*
 * mensajes.h
 *
 *  Created on: 13/11/2012
 *      Author: gonzalo
 */

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

#endif /* MENSAJES_H_ */
