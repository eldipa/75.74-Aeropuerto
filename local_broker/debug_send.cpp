#include "messagequeue.h"
#include <cstring>
#include <cstdio>
#include <iostream>
#include "group_interface.h"

int main() {
	char data[100];
	GroupInterface grupo("/tmp/test", char(0));

	for (int i = 0; i < 10; i++) {

		sprintf(data,"hola %d",i);

		std::cout << "enviado: " << data << std::endl;

		grupo.push(data, sizeof(data));

	}
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
	return -1;
}
