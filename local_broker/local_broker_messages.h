/*
 * local_broker_messages.h
 *
 *  Created on: 06/02/2013
 *      Author: gonzalo
 */

#ifndef LOCAL_BROKER_MESSAGES_H_
#define LOCAL_BROKER_MESSAGES_H_

#include "api_constants.h"

typedef struct {
	long mtype;
	char recurso [MAX_NOMBRE_RECURSO];
} traspaso_token_t;

typedef struct {
	long mtype;
	long id_secuencia_vista;
	char vista_grupo [MAX_NOMBRE_RECURSO];
} traspaso_vista_t;

typedef enum {
	T_HELLO, T_VIEW_CHANGE, T_UPDATE_MEMORY, T_ALREADY_INITIALIZED
} MENSAJE_GRUPO;

typedef struct {
	MENSAJE_GRUPO tipomsg;
	char nombre_recurso [MAX_NOMBRE_RECURSO];
	char id_broker_emisor[MAX_NOMBRE_RECURSO];
	char data [MAX_DATA_SIZE];
} mensaje_grupos_t;

#endif /* LOCAL_BROKER_CONSTANTS_H_ */
