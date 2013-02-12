/*
 * mensajes_de_red.h
 *
 *  Created on: 06/02/2013
 *      Author: gonzalo
 */

#ifndef MENSAJES_DE_RED_H_
#define MENSAJES_DE_RED_H_

#define DATA_SIZE 512
#define MAX_NAME_SIZE 30
#define TOKEN_HEADER_SIZE (MAX_NAME_SIZE)

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
	char datos [DATA_SIZE];
} mensajes_local_broker_t;

typedef struct {
	char recurso [MAX_NAME_SIZE];
	char datos [DATA_SIZE];
} mensajes_local_broker_token_t;

}
#endif /* MENSAJES_DE_RED_H_ */
