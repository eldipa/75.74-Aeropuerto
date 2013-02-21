/*
 * InitParser.cpp
 *
 *  Created on: 14/02/2013
 *      Author: gonzalo
 */

#include "init_parser.h"

#include "local_broker_constants.h"

InitParser::InitParser() {
	size = 100;
	linea = new char [size + 1];
}

InitParser::~InitParser() {
	delete [] linea;
}

bool InitParser::cmp(const char *a, const char *b) {
	return strcmp(a, b) == 0 ? true : false;
}

int InitParser::parse_int_val(const char * val) {
	int pos = 0;
	int len = strlen(val);
	if (val [pos] == '-') {
		pos++;
	}
	while (val [pos] >= '0' && val [pos] <= '9' && val [pos] != '\0') {
		pos++;
	}
	if (pos == len) {
		return atoi(val);
	}
	if (cmp(val, "CAPACIDAD_CINTA_CHECKIN")) {
		return CAPACIDAD_CINTA_CHECKIN;
	} else if (cmp(val, "CAPACIDAD_CINTA_SCANNER")) {
		return CAPACIDAD_CINTA_SCANNER;
	} else if (cmp(val, "CAPACIDAD_CINTA_CENTRAL")) {
		return CAPACIDAD_CINTA_CENTRAL;
	} else if (cmp(val, "CANTIDAD_MAX_PRODUCTORES_CINTA_CHECKIN")) {
		return CANTIDAD_MAX_PRODUCTORES_CINTA_CHECKIN;
	} else if (cmp(val, "CANTIDAD_MAX_CONSUMIDORES_CINTA_CHECKIN")) {
		return CANTIDAD_MAX_CONSUMIDORES_CINTA_CHECKIN;
	} else if (cmp(val, "CANTIDAD_MAX_PRODUCTORES_CINTA_SCANNER")) {
		return CANTIDAD_MAX_PRODUCTORES_CINTA_SCANNER;
	} else if (cmp(val, "CANTIDAD_MAX_CONSUMIDORES_CINTA_SCANNER")) {
		return CANTIDAD_MAX_CONSUMIDORES_CINTA_SCANNER;
	} else if (cmp(val, "CANTIDAD_MAX_PRODUCTORES_CINTA_CONTENEDOR")) {
		return CANTIDAD_MAX_PRODUCTORES_CINTA_CONTENEDOR;
	} else if (cmp(val, "CANTIDAD_MAX_CONSUMIDORES_CINTA_CONTENEDOR")) {
		return CANTIDAD_MAX_CONSUMIDORES_CINTA_CONTENEDOR;
	} else if (cmp(val, "CANTIDAD_MAX_PRODUCTORES_CINTA_CENTRAL")) {
		return CANTIDAD_MAX_PRODUCTORES_CINTA_CENTRAL;
	} else if (cmp(val, "CANTIDAD_MAX_CONSUMIDORES_CINTA_CENTRAL")) {
		return CANTIDAD_MAX_CONSUMIDORES_CINTA_CENTRAL;
		//} else if (cmp(val, "CANTIDAD_DE_CONSUMIDORES_PPAL")) {
		//	return CANTIDAD_DE_CONSUMIDORES_PPAL;
	} else if (cmp(val, "CAPACIDAD_CINTA_CONTENEDOR")) {
		return CAPACIDAD_CINTA_CONTENEDOR;
	} else if (cmp(val, "MAX_SCANNERS")) {
		return MAX_SCANNERS;
	} else if (cmp(val, "ZONAS_POR_ROBOT_DESPACHO")) {
		return ZONAS_POR_ROBOT_DESPACHO;
	} else if (cmp(val, "MAX_ROBOTS_INTERCARGO_ESPERANDO_POR_ZONAS")) {
		return MAX_ROBOTS_INTERCARGO_ESPERANDO_POR_ZONAS;
	} else if (cmp(val, "MAX_CLIENTS")) {
		return MAX_CLIENTS;
	} else if (cmp(val, "TAMANIO_MEMORIA_CINTA_PRINCIPAL")) {
		return TAMANIO_MEMORIA_CINTA_PRINCIPAL;
	} else if (cmp(val, "TAMANIO_MEMORIA_CINTA_SCANNER")) {
		return TAMANIO_MEMORIA_CINTA_SCANNER;
	} else if (cmp(val, "TAMANIO_MEMORIA_CONTROL_CINTA_SCANNER")) {
		return TAMANIO_MEMORIA_CONTROL_CINTA_SCANNER;
	} else if (cmp(val, "TAMANIO_MEMORIA_CINTA_CARGA")) {
		return TAMANIO_MEMORIA_CINTA_CARGA;
	} else if (cmp(val, "TAMANIO_MEMORIA_CINTA_CHECKIN")) {
		return TAMANIO_MEMORIA_CINTA_CHECKIN;
	} else if (cmp(val, "TAMANIO_MEMORIA_ZONAS_ASIGNADAS")) {
		return TAMANIO_MEMORIA_ZONAS_ASIGNADAS;
	}else if (cmp(val, "MAX_INTERCARGOS_ESPERANDO")) {
		return MAX_INTERCARGOS_ESPERANDO;
	} else if (cmp(val, "CANT_MUTEX_CENTRAL")) {
		return CANT_MUTEX_CENTRAL;
	} else if (cmp(val, "Q_ZONAS")) {
		return Q_ZONAS;
	} else if (cmp(val, "Q_PUESTOS_CHECKIN")) {
		return Q_PUESTOS_CHECKIN;
	} else if (cmp(val, "Q_CONTENEDORES")) {
		return Q_CONTENEDORES;
	} else if (cmp(val, "Q_TRASBORDO_LISTO")) {
		return Q_TRASBORDO_LISTO;
	} else if (cmp(val, "Q_CHECKINS_CERRADO")) {
		return Q_CHECKINS_CERRADO;
	} else if (cmp(val, "Q_CHECKINS_HABILITADOS")) {
		return Q_CHECKINS_HABILITADOS;
	} else if (cmp(val, "MTX_CENTRAL")) {
		return MTX_CENTRAL;
	}
	return 0;
}

size_t InitParser::parse_size_t_val(const char * val) {
	int pos = 0;
	int len = strlen(val);
	while (val [pos] >= '0' && val [pos] <= '9' && val [pos] != '\0') {
		pos++;
	}
	if (pos == len) {
		return atoi(val);
	}
	if (cmp(val, "CAPACIDAD_CINTA_CHECKIN")) {
		return CAPACIDAD_CINTA_CHECKIN;
	} else if (cmp(val, "CAPACIDAD_CINTA_SCANNER")) {
		return CAPACIDAD_CINTA_SCANNER;
	} else if (cmp(val, "CAPACIDAD_CINTA_CENTRAL")) {
		return CAPACIDAD_CINTA_CENTRAL;
	} else if (cmp(val, "CANTIDAD_MAX_PRODUCTORES_CINTA_CHECKIN")) {
		return CANTIDAD_MAX_PRODUCTORES_CINTA_CHECKIN;
	} else if (cmp(val, "CANTIDAD_MAX_CONSUMIDORES_CINTA_CHECKIN")) {
		return CANTIDAD_MAX_CONSUMIDORES_CINTA_CHECKIN;
	} else if (cmp(val, "CANTIDAD_MAX_PRODUCTORES_CINTA_SCANNER")) {
		return CANTIDAD_MAX_PRODUCTORES_CINTA_SCANNER;
	} else if (cmp(val, "CANTIDAD_MAX_CONSUMIDORES_CINTA_SCANNER")) {
		return CANTIDAD_MAX_CONSUMIDORES_CINTA_SCANNER;
	} else if (cmp(val, "CANTIDAD_MAX_PRODUCTORES_CINTA_CONTENEDOR")) {
		return CANTIDAD_MAX_PRODUCTORES_CINTA_CONTENEDOR;
	} else if (cmp(val, "CANTIDAD_MAX_CONSUMIDORES_CINTA_CONTENEDOR")) {
		return CANTIDAD_MAX_CONSUMIDORES_CINTA_CONTENEDOR;
	} else if (cmp(val, "CANTIDAD_MAX_PRODUCTORES_CINTA_CENTRAL")) {
		return CANTIDAD_MAX_PRODUCTORES_CINTA_CENTRAL;
	} else if (cmp(val, "CANTIDAD_MAX_CONSUMIDORES_CINTA_CENTRAL")) {
		return CANTIDAD_MAX_CONSUMIDORES_CINTA_CENTRAL;
		//} else if (cmp(val, "CANTIDAD_DE_CONSUMIDORES_PPAL")) {
		//	return CANTIDAD_DE_CONSUMIDORES_PPAL;
	} else if (cmp(val, "CAPACIDAD_CINTA_CONTENEDOR")) {
		return CAPACIDAD_CINTA_CONTENEDOR;
	} else if (cmp(val, "TAMANIO_MEMORIA_CINTA_PRINCIPAL")) {
		return TAMANIO_MEMORIA_CINTA_PRINCIPAL;
	} else if (cmp(val, "TAMANIO_MEMORIA_CINTA_SCANNER")) {
		return TAMANIO_MEMORIA_CINTA_SCANNER;
	} else if (cmp(val, "TAMANIO_MEMORIA_CONTROL_CINTA_SCANNER")) {
		return TAMANIO_MEMORIA_CONTROL_CINTA_SCANNER;
	} else if (cmp(val, "TAMANIO_MEMORIA_CINTA_CARGA")) {
		return TAMANIO_MEMORIA_CINTA_CARGA;
	} else if (cmp(val, "TAMANIO_MEMORIA_CINTA_CHECKIN")) {
		return TAMANIO_MEMORIA_CINTA_CHECKIN;
	} else if (cmp(val, "TAMANIO_MEMORIA_ZONAS_ASIGNADAS")) {
		return TAMANIO_MEMORIA_ZONAS_ASIGNADAS;
	}else if (cmp(val, "MAX_SCANNERS")) {
		return MAX_SCANNERS;
	} else if (cmp(val, "ZONAS_POR_ROBOT_DESPACHO")) {
		return ZONAS_POR_ROBOT_DESPACHO;
	} else if (cmp(val, "MAX_ROBOTS_INTERCARGO_ESPERANDO_POR_ZONAS")) {
		return MAX_ROBOTS_INTERCARGO_ESPERANDO_POR_ZONAS;
	} else if (cmp(val, "MAX_CLIENTS")) {
		return MAX_CLIENTS;
	} else if (cmp(val, "MAX_INTERCARGOS_ESPERANDO")) {
		return MAX_INTERCARGOS_ESPERANDO;
	} else if (cmp(val, "CANT_MUTEX_CENTRAL")) {
		return CANT_MUTEX_CENTRAL;
	} else if (cmp(val, "Q_ZONAS")) {
		return Q_ZONAS;
	} else if (cmp(val, "Q_PUESTOS_CHECKIN")) {
		return Q_PUESTOS_CHECKIN;
	} else if (cmp(val, "Q_CONTENEDORES")) {
		return Q_CONTENEDORES;
	} else if (cmp(val, "Q_TRASBORDO_LISTO")) {
		return Q_TRASBORDO_LISTO;
	} else if (cmp(val, "Q_CHECKINS_CERRADO")) {
		return Q_CHECKINS_CERRADO;
	} else if (cmp(val, "Q_CHECKINS_HABILITADOS")) {
		return Q_CHECKINS_HABILITADOS;
	} else if (cmp(val, "MTX_CENTRAL")) {
		return MTX_CENTRAL;
	}
	return 0;
}

bool InitParser::is_vector() {
	return this->cant_vector == 1;
}

int InitParser::vector() {
	return this->cant_vector;
}

int InitParser::int_val() {
	return valor_int;
}

T_TYPE InitParser::type() {
	return tipo;
}

void InitParser::parse_type(const char * type) {
	if (strcmp(type, "int") == 0) {
		this->tipo = T_INT;
	}
}

void InitParser::parse_line(const char * line) {
	int len;
	len = strlen(line);

	if (len > size) {
		delete [] linea;
		linea = new char [len + 1];
		size = len + 1;
	}

	strcpy(linea, line);

	char * tipo;
	char * vector = NULL;
	char * valor;

	int pos = 0;

	while (linea [pos] != ' ') {
		pos++;
	}
	linea [pos++] = '\0';
	tipo = linea;

	while (linea [pos] == ' ') {
		pos++;
	}

	if (linea [pos] == '[') {
		vector = &(linea [pos + 1]);
		while (linea [pos] != ' ' && linea [pos] != ']') {
			pos++;
		}
		linea [pos++] = '\0';
		while (linea [pos] == ' ') {
			pos++;
		}
		valor = &(linea [pos]);
	} else {
		valor = &(linea [pos]);
	}

	parse_type(tipo);

	if (vector == NULL) {
		cant_vector = 1;
	} else {
		cant_vector = parse_int_val(vector);
	}

	switch (this->tipo) {
		case T_INT:
			valor_int = parse_int_val(valor);
			break;
		default:
			break;
	}

}
