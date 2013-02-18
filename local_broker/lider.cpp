/*
 * Lider.cpp
 *
 *  Created on: 17/02/2013
 *      Author: gonzalo
 */

#include "lider.h"
#include "oserror.h"
#include <iostream>
#include <cstdlib>

Lider::Lider(const std::string directorio_de_trabajo, const std::string & nombre_grupo, char id_grupo,
	const std::string & nombre_broker)
	: cola_token_manager(std::string(directorio_de_trabajo).append(PATH_COLA_TOKEN_MANAGER).c_str(), char(0)),
		cola_lider(std::string(directorio_de_trabajo).append(PATH_COLA_LEADER).c_str(), char(0)),
		grupo_remoto(std::string(directorio_de_trabajo).append(PATH_COLAS_BROKERS).c_str(), id_grupo),
		grupo(directorio_de_trabajo, nombre_grupo), broker_local(nombre_broker)
{
	cant_nodos = 0;
}

Lider::~Lider() {

}

void Lider::send_msg() {
	grupo_remoto.push((char *)&mensaje_envio, sizeof(mensajes::mensajes_local_broker_group_t));
}

void Lider::wait_msg() {
	cola_lider.pull(&mensaje, sizeof(mensajes::mensajes_local_broker_group_t), 0);
}

void Lider::do_loop() {

}

void Lider::run() {
	bool leave = false;
	//grupo.release_token(&this->cola_token_manager);
	int * entero;
	int * entero_envio;
	bool token_recuperado;
	entero = (int *)(mensaje.data.data);
	entero_envio = (int *)mensaje_envio.data;

	mensaje_envio.cantidad_bytes_total = 1;
	*entero_envio = 1;
	mensaje_envio.numero_de_mensaje = 1;
	mensaje_envio.tipo = mensajes::GROUP_DISCOVER;

	send_msg();

	do {

	} while (mensaje.data.tipo != mensajes::GROUP_DISCOVER);

	do {
		try {
			wait_msg();

			if (mensaje.data.tipo == mensajes::GROUP_DISCOVER) {
				cant_nodos = *entero;
			} else if (mensaje.data.tipo == mensajes::TOKEN_LOST) {
				*entero_envio = *entero;
				token_recuperado = false;
				do {
					(*entero_envio)--;
					if (*entero_envio > 1) {
						mensaje_envio.tipo = mensajes::TOKEN_RECOVER;
						send_msg();
						wait_msg();
						if(strcmp(mensaje.data.data, "OK") == 0){
							token_recuperado = true;
						}
					} else {
						token_recuperado = true;
						grupo.release_token(&cola_token_manager);
					}
				} while (!token_recuperado);
			}

		} catch (OSError & error) {
			leave = true;
		}
	} while (!leave);
}

int main(int argc, char * argv []) {
	char id;

	//print_args(argc, argv);

	if (argc < 5) {
		std::cerr << "Falta el directorio de trabajo, el id, el nombre del recurso, el nombre_broker_local"
			<< std::endl;
		return -1;
	}
	id = atoi(argv [2]);

	Lider lider(argv [1], argv [3], id, argv [4]);

	lider.run();

	/*char data [100];
	 //GroupInterface grupo_remoto(lock_file.c_str(), id);
	 Grupo grupo(argv [1], argv [3]);
	 grupo.join(argv [4]);

	 for (int i = 0 ; i < 10 ; i++) {

	 //grupo_remoto.pull(data, sizeof(data));

	 std::cout << "recibido: " << data << std::endl;

	 }*/

	/*
	 MessageQueue outbound("/tmp/test", char(128));
	 MessageQueue inbound("/tmp/test", char(0));
	 mensaje_t mensaje;

	 mensaje.mtype = 1;

	 for (int i = 0; i < 10; i++) {
	 sprintf(mensaje.msg, "hola%d", i);

	 std::cout << "enviado:" << mensaje.msg << std::endl;

	 outbound.push(&mensaje, size_t(sizeof(mensaje_t) - sizeof(long)));
	 }*/
	return 0;
}
