/*
 * interfaces.h
 *
 *  Created on: 17/02/2013
 *      Author: gonzalo
 */

#ifndef INTERFACES_H_
#define INTERFACES_H_

#include <arpa/inet.h>

void get_local_ip(char local_ip[INET_ADDRSTRLEN], char * iface_name);
void get_bcast_ip(char bcast_ip[INET_ADDRSTRLEN], char * iface_name);
void get_netmask(char netmask[INET_ADDRSTRLEN], char * iface_name);

#endif /* INTERFACES_H_ */
