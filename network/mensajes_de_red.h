/*
 * mensajes_de_red.h
 *
 *  Created on: 06/02/2013
 *      Author: gonzalo
 */

#ifndef MENSAJES_DE_RED_H_
#define MENSAJES_DE_RED_H_

#include "constants.h"
#include <cstring>

#define DATA_SIZE 512
#define TOKEN_HEADER_SIZE (MAX_NOMBRE_RECURSO)

namespace mensajes {

typedef enum {
	REGISTER, JOIN, LEAVE, TOKEN_FREE, END_JOIN
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
	TOKEN_DELIVER, GROUP_DISCOVER
} GROUP_MSG_T;

typedef struct {
	GROUP_MSG_T tipo;
	int numero_de_mensaje;
	size_t cantidad_bytes_total;
	char data[GROUP_MSG_DATA_SIZE];
} mensajes_local_broker_group_t;

}
#endif /* MENSAJES_DE_RED_H_ */
