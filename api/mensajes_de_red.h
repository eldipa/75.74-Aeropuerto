/*
 * mensajes_de_red.h
 *
 *  Created on: 06/02/2013
 *      Author: gonzalo
 */

#ifndef MENSAJES_DE_RED_H_
#define MENSAJES_DE_RED_H_

namespace mensajes {

typedef enum {
	REGISTER, JOIN, LEAVE, TOKEN_FREE, END_JOIN
} peticiones_t;

typedef enum {
	OK, ERROR, TOKEN_DELIVER
} respuestas_t;

typedef struct {
	peticiones_t peticion;
	respuestas_t respuesta;
	char datos [200];
} mensajes_local_broker_t;

}
#endif /* MENSAJES_DE_RED_H_ */
