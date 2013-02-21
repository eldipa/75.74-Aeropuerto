/*
 * mensajes_de_red.h
 *
 *  Created on: 06/02/2013
 *      Author: gonzalo
 */

#ifndef MENSAJES_DE_RED_H_
#define MENSAJES_DE_RED_H_

#include "globalconstants.h"
#include <cstring>

#define DATA_SIZE 256
#define TOKEN_HEADER_SIZE (MAX_NOMBRE_RECURSO)

namespace mensajes {

typedef enum {
	REGISTER, JOIN, JOIN_FORK, LEAVE, TOKEN_FREE, END_JOIN
} peticiones_t;

typedef enum {
	OK, ERROR
} respuestas_t;

typedef struct {
	peticiones_t peticion;
	respuestas_t respuesta;
	char datos [DATA_SIZE];
} mensajes_local_broker_t;

typedef struct {
	char recurso [MAX_NOMBRE_RECURSO];
	char datos [DATA_SIZE];
} mensajes_local_broker_token_t;

typedef enum {
	LEADER = 0, TOKEN_DELIVER = 1, GROUP_DISCOVER = 2, TOKEN_LOST = 3, INITALIZATION = 4, TOKEN_RECOVER = 5
} GROUP_MSG_T;

typedef struct {
	GROUP_MSG_T tipo;
	int numero_de_mensaje;
	size_t cantidad_bytes_total;
	char data [DATA_SIZE];
} mensajes_local_broker_group_t;

typedef struct {
	long mtype;
	mensajes_local_broker_group_t data;
} mensajes_local_broker_lider_t;

}
#endif /* MENSAJES_DE_RED_H_ */
