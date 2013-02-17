/*
 * hostname.cpp
 *
 *  Created on: 17/02/2013
 *      Author: gonzalo
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>

void get_hostname(char * hostname, char ipv4 [INET_ADDRSTRLEN]) {
	struct hostent *he;
	struct in_addr ipv4addr;

	inet_pton(AF_INET, ipv4, &ipv4addr);
	he = gethostbyaddr(&ipv4addr, sizeof ipv4addr, AF_INET);
	//printf("Host name: %s\n", he->h_name);
	strcpy(hostname,he->h_name);

	//char** tmp = he->h_aliases;
	/*while (*tmp != NULL) {
		printf("Alias: %s\n", *tmp);
		tmp++;
	}*/
}

/*
int main(int argc, char* argv []) {
	if (argc < 2) {
		printf("Usage: %s <ipaddress>\n", argv [0]);
		return 1;
	}

	struct hostent *he;
	struct in_addr ipv4addr;

	inet_pton(AF_INET, argv [1], &ipv4addr);
	he = gethostbyaddr(&ipv4addr, sizeof ipv4addr, AF_INET);
	printf("Host name: %s\n", he->h_name);

	char** tmp = he->h_aliases;
	while (*tmp != NULL) {
		printf("Alias: %s\n", *tmp);
		tmp++;
	}
}*/
