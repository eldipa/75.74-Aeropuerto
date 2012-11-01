#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include "log.h"
#include "cinta.h"
#include "equipaje.h"
#include "constants.h"


/* aca hay que ver como manejamos los directorios.*/
char *args_robot_checkin[] = { (char*)"./robot_checkin", 
                               (char*)"0", 
                               (char*)"/home/raul/mem.txt", 
                               (char*)"/home/raul/sem.txt", 
                               (char*)"/home/raul/sem.txt", 
                               (char*)"/home/raul/sem.txt", 
                               NULL };

char *args_puesto_checkin[] = { (char*)"./puesto_checkin", 
                                (char*)"0", 
                                (char*)"/home/raul/mem.txt", 
                                (char*)"/home/raul/sem.txt", 
                                NULL };


/*
 * Crea un puesto de checkin comunicado con un robot_scanner a travez de una cinta.
 **/

int main() {
	pid_t pid = 0;

   Cinta< Equipaje, CAPACIDAD_CINTA_CHECKIN > checkin_to_scanner(PUESTOS_CHECKIN_POR_CINTA,
                                                                 ROBOT_CHECKIN_POR_CINTA, 
                                                                 "/home/raul/mem.txt",
                                                                 "/home/raul/sem.txt",
                                                                 true);
   
   Log::info("iniciando simulación...");

   Log::info("Lanzo proceso puesto_checkin(%d)...", pid);
	if ( (pid = fork()) == 0 ) {
		if( execv( "./puesto_checkin", args_puesto_checkin ) == -1 ) {
			perror("error:");
	 		exit(0);
		}
	}

   Log::info("Lanzo proceso robot_checkin(%d)...", pid);
	if ( (pid = fork()) == 0 ) {
		if( execv( "./robot_checkin", args_robot_checkin ) == -1 ) {
			perror("error:");
			exit(0);
		}
	}
   	
   sleep( TIEMPO_SIMULACION );
   return 0;
}
