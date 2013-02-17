/*
 * Interfaces.cpp
 *
 *  Created on: 17/02/2013
 *      Author: gonzalo
 */
//#include <stdio.h>
#include <sys/types.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>

void get_local_ip(char local_ip [INET_ADDRSTRLEN], char * iface_name) {
	struct ifaddrs * ifAddrStruct = NULL;
	struct ifaddrs * ifa = NULL;
	void * tmpAddrPtr = NULL;

	getifaddrs(&ifAddrStruct);

	for (ifa = ifAddrStruct; ifa != NULL ; ifa = ifa->ifa_next) {
		if (ifa->ifa_addr->sa_family == AF_INET) { // check it is IP4
			if (strncmp(ifa->ifa_name, iface_name, strlen(ifa->ifa_name)) == 0) {
				tmpAddrPtr = &((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
				inet_ntop(AF_INET, tmpAddrPtr, local_ip, INET_ADDRSTRLEN);
				//printf("%s IP Address %s\n", ifa->ifa_name, local_ip);
				break;
			}

		}
	}
	if (ifAddrStruct != NULL)
		freeifaddrs(ifAddrStruct);
}

void get_bcast_ip(char bcast_ip [INET_ADDRSTRLEN], char * iface_name) {
	struct ifaddrs * ifAddrStruct = NULL;
	struct ifaddrs * ifa = NULL;
	void * tmpAddrBcast = NULL;
	in_addr bcast;
	bcast.s_addr = uint32_t(0xffffffff);

	getifaddrs(&ifAddrStruct);

	for (ifa = ifAddrStruct; ifa != NULL ; ifa = ifa->ifa_next) {
		if (ifa->ifa_addr->sa_family == AF_INET) { // check it is IP4
			// is a valid IP4 Address
			if (strncmp(ifa->ifa_name, iface_name, strlen(ifa->ifa_name)) == 0) {
				bcast.s_addr = bcast.s_addr ^ ((struct sockaddr_in *)ifa->ifa_netmask)->sin_addr.s_addr;
				bcast.s_addr = bcast.s_addr | ((struct sockaddr_in *)ifa->ifa_addr)->sin_addr.s_addr;
				tmpAddrBcast = &bcast;
				inet_ntop(AF_INET, tmpAddrBcast, bcast_ip, INET_ADDRSTRLEN);
				//printf("%s Broadcast: %s\n", ifa->ifa_name, bcast_ip);
			}

		}
	}
	if (ifAddrStruct != NULL)
		freeifaddrs(ifAddrStruct);
}

void get_netmask(char netmask [INET_ADDRSTRLEN], char * iface_name) {
	struct ifaddrs * ifAddrStruct = NULL;
	struct ifaddrs * ifa = NULL;
	void * tmpAddrMask = NULL;

	getifaddrs(&ifAddrStruct);

	for (ifa = ifAddrStruct; ifa != NULL ; ifa = ifa->ifa_next) {
		if (ifa->ifa_addr->sa_family == AF_INET) { // check it is IP4
			// is a valid IP4 Address
			if (strncmp(ifa->ifa_name, iface_name, strlen(ifa->ifa_name)) == 0) {
				tmpAddrMask = &((struct sockaddr_in *)ifa->ifa_netmask)->sin_addr;
				inet_ntop(AF_INET, tmpAddrMask, netmask, INET_ADDRSTRLEN);
				//printf("%s Mask %s \n", ifa->ifa_name, netmask);
			}
		}
	}
	if (ifAddrStruct != NULL)
		freeifaddrs(ifAddrStruct);
}

/*
int main() {
	struct ifaddrs * ifAddrStruct = NULL;
	struct ifaddrs * ifa = NULL;
	void * tmpAddrPtr = NULL;
	void * tmpAddrMask = NULL;
	void * tmpAddrBcast = NULL;
	in_addr bcast;
	bcast.s_addr = uint32_t(0xffffffff);
	char local_ip [INET_ADDRSTRLEN];

	get_netmask(local_ip,(char *)"wlan0");

	getifaddrs(&ifAddrStruct);

	for (ifa = ifAddrStruct; ifa != NULL ; ifa = ifa->ifa_next) {
		if (ifa->ifa_addr->sa_family == AF_INET) { // check it is IP4
			// is a valid IP4 Address
			char addressBuffer [INET_ADDRSTRLEN];
			char maskBuffer [INET_ADDRSTRLEN];
			char bcastBuffer [INET_ADDRSTRLEN];
			tmpAddrPtr = &((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
			tmpAddrMask = &((struct sockaddr_in *)ifa->ifa_netmask)->sin_addr;
			bcast.s_addr = bcast.s_addr ^ ((struct sockaddr_in *)ifa->ifa_netmask)->sin_addr.s_addr;
			bcast.s_addr = bcast.s_addr | ((struct sockaddr_in *)ifa->ifa_addr)->sin_addr.s_addr;
			tmpAddrBcast = &bcast;
			inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
			inet_ntop(AF_INET, tmpAddrMask, maskBuffer, INET_ADDRSTRLEN);
			inet_ntop(AF_INET, tmpAddrBcast, bcastBuffer, INET_ADDRSTRLEN);
			if (strncmp(ifa->ifa_name, "wlan0", 6)==0)
				printf("%s IP Address %s Mask %s Broadcast: %s\n", ifa->ifa_name, addressBuffer, maskBuffer,
					bcastBuffer);

		} else if (ifa->ifa_addr->sa_family == AF_INET6) { // check it is IP6
			// is a valid IP6 Address
			tmpAddrPtr = &((struct sockaddr_in6 *)ifa->ifa_addr)->sin6_addr;
			char addressBuffer [INET6_ADDRSTRLEN];
			inet_ntop(AF_INET6, tmpAddrPtr, addressBuffer, INET6_ADDRSTRLEN);
			printf("%s IP Address %s\n", ifa->ifa_name, addressBuffer);
		}
	}
	if (ifAddrStruct != NULL)
		freeifaddrs(ifAddrStruct);
	return 0;
}*/
