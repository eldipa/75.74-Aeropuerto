/*
 * GroupMultiplexer.cpp
 *
 *  Created on: 03/03/2013
 *      Author: gonzalo
 */

#include "group_multiplexer.h"
#include "valueerror.h"

GroupMultiplexer::GroupMultiplexer(const char *absolute_path)
	: interfaz_grupos(absolute_path, char(0)), cola_espera_grupos(absolute_path, char(1), 0664, true),
		memoria_grupos(absolute_path, char(2), TAMANIO_TABLA_COLAS_ASIGNADAS, 0664, true, false),
		mutex(std::vector<unsigned short>(1, 1), absolute_path, char(3), 0664)
{
	mutex.wait_on(0);
	lider = reinterpret_cast<int *>(memoria_grupos.memory_pointer());

	grupos_creados [0] = reinterpret_cast<char *>(lider + 1);
	for (int i = 1 ; i < MAX_GRUPOS ; i++) {
		grupos_creados [i] = grupos_creados [i - 1] + sizeof(char) * MAX_NOMBRE_RECURSO;
		*grupos_creados [i] = '\0';
	}
	*lider = 0;
	mutex.signalize(0);

}

GroupMultiplexer::GroupMultiplexer(const char *absolute_path, const char * nombre_grupo)
	: interfaz_grupos(absolute_path, char(0)), cola_espera_grupos(absolute_path, char(1)),
		memoria_grupos(absolute_path, char(2), 0, 0, false, false), mutex(absolute_path, char(3), 1, 0664)
{
	lider = reinterpret_cast<int *>(memoria_grupos.memory_pointer());

	grupos_creados [0] = reinterpret_cast<char *>(lider + 1);
	for (int i = 1 ; i < MAX_GRUPOS ; i++) {
		grupos_creados [i] = grupos_creados [i - 1] + sizeof(char) * MAX_NOMBRE_RECURSO;
	}

	mutex.wait_on(0);

	for (int i = 0 ; i < MAX_GRUPOS ; i++) {
		if (*grupos_creados [i] != '\0' && strncmp(grupos_creados [i], nombre_grupo, MAX_NOMBRE_RECURSO) == 0) {
			numero_cola_asignada = i + 1;
			break;
		} else if (*grupos_creados [i] == '\0') {
			strncpy(grupos_creados [i], nombre_grupo, MAX_NOMBRE_RECURSO);
			numero_cola_asignada = i + 1;
			break;
		}
	}
	if (*lider == 1) {
		mensajes::mensajes_local_broker_lider_t mensaje_cola;
		mensaje_cola.mtype = numero_cola_asignada;
		mensaje_cola.data.tipo = mensajes::LEADER;
		cola_espera_grupos.push(&mensaje_cola, sizeof(mensajes::mensajes_local_broker_group_t));
	}

	mutex.signalize(0);
}

GroupMultiplexer::~GroupMultiplexer() {
}

void GroupMultiplexer::push(mensajes::mensajes_local_broker_group_t & mensaje) {
	interfaz_grupos.push((char *)&mensaje, sizeof(mensajes::mensajes_local_broker_group_t));
}

ssize_t GroupMultiplexer::pull(mensajes::mensajes_local_broker_group_t & mensaje) {
	size_t recv;
	mensajes::mensajes_local_broker_lider_t mensaje_cola;
	recv = cola_espera_grupos.pull(&mensaje_cola, sizeof(mensajes::mensajes_local_broker_group_t),
		numero_cola_asignada);
	mensaje = mensaje_cola.data;
	return recv;
}

void GroupMultiplexer::forward_message() {
	bool grupo_asignado = false;
	mensajes::mensajes_local_broker_group_t mensaje;
	mensajes::mensajes_local_broker_lider_t mensaje_cola;

	interfaz_grupos.pull((char *)&mensaje, sizeof(mensajes::mensajes_local_broker_group_t));

	Log::debug("Mensaje recibido de grupo %s (%d)\n", mensaje.nombre_grupo, mensaje.tipo);

	if (mensaje.tipo == mensajes::LEADER) {
		Log::debug("Recibido Mensaje Leader\n");
		mutex.wait_on(0);
		*lider = 1;
		for (int i = 0 ; i < MAX_GRUPOS ; i++) {
			if (*grupos_creados [i] != '\0') {
				mensaje_cola.mtype = i + 1;
				mensaje_cola.data = mensaje;
				cola_espera_grupos.push(&mensaje_cola, sizeof(mensajes::mensajes_local_broker_group_t));
			}
		}
		mutex.signalize(0);
	} else {
		if (grupos.count(std::string(mensaje.nombre_grupo)) > 0) {
			numero_cola_asignada = grupos.at(std::string(mensaje.nombre_grupo));
			grupo_asignado = true;
		} else {
			mutex.signalize(0);
			for (int i = 0 ; i < MAX_GRUPOS ; i++) {
				if (*grupos_creados [i] != '\0'
					&& strncmp(grupos_creados [i], mensaje.nombre_grupo, MAX_NOMBRE_RECURSO) == 0)
				{
					numero_cola_asignada = i + 1;
					grupo_asignado = true;
					grupos.insert(std::pair<std::string, long>(std::string(grupos_creados [i]), numero_cola_asignada));
					break;
				} else if (*grupos_creados [i] == '\0') {
					break;
				}
			}
			mutex.signalize(0);
		}

		if (grupo_asignado) {
			mensaje_cola.data = mensaje;
			mensaje_cola.mtype = numero_cola_asignada;
			cola_espera_grupos.push(&mensaje_cola, sizeof(mensajes::mensajes_local_broker_group_t));
		} else {
			interfaz_grupos.push((char *)&mensaje, sizeof(mensajes::mensajes_local_broker_group_t));
		}
	}
}
