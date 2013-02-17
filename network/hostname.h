/*
 * hostname.h
 *
 *  Created on: 17/02/2013
 *      Author: gonzalo
 */

#ifndef HOSTNAME_H_
#define HOSTNAME_H_

#include <arpa/inet.h>

void get_hostname(char * hostname, char ipv4[INET_ADDRSTRLEN]);

#endif /* HOSTNAME_H_ */
