#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "constants.h"
#include "log.h"

#include "process.h"
#include <list>

typedef struct t_task_vuelo {
   int time;
   int num_vuelo;
} tTaskVuelo;

bool compare_time (tTaskVuelo t1, tTaskVuelo t2) {
   return ( t1.time < t2.time );
}

void lanzar_vuelo(tTaskVuelo, const char* dir);
void cargar_vuelos( std::list<tTaskVuelo>& );

int main(int argc, char *argv[]) try {
   if (argc < 1) {
      Log::crit("Insuficientes parametros para scheduler_vuelos, se esperaba (directorio_de_trabajo)\n");
      return (1);
   }
   
   Log::info("Comenzando scheduler_vuelos a lanzar vuelos");
      
   int time = 0;
   std::list<tTaskVuelo> vuelos;
   cargar_vuelos(vuelos);
   vuelos.sort(compare_time);
   
   while( !vuelos.empty() ) {

      while( (!vuelos.empty()) && (vuelos.front().time <= time ) ){
         lanzar_vuelo(vuelos.front(), argv[1]);
         vuelos.pop_front();
      }
      sleep(1);
      time += 1;
   }

   Log::info("Fin scheduler, no hay mas vuelos");
} catch(const std::exception &e) {
   Log::crit("%s", e.what());
} catch(...) {
   Log::crit("Critical error. Unknow exception at the end of the 'main' function.");
}

void cargar_vuelos(std::list<tTaskVuelo>& list) {
   char primera_linea[255];
	FILE * file_vuelos;
   tTaskVuelo t;

	file_vuelos = fopen("./entrada/vuelos.csv", "rt");
	fscanf(file_vuelos, "%[^\n]s\n", primera_linea);

	while (fscanf(file_vuelos, "%d:%d\n",&t.time, &t.num_vuelo) != EOF) {
      Log::debug("push vuelo %d", t.num_vuelo);
      list.push_front(t);
	}

	fclose(file_vuelos);

}

void lanzar_vuelo(tTaskVuelo t, const char* dir) {
   char buffer_num_vuelo[6];
   char processName[255];

	Log::info("Lanzando vuelo %d", t.num_vuelo);

	std::stringstream sstr;
	sstr << t.num_vuelo;
	strcpy(buffer_num_vuelo, sstr.str().c_str());

   sstr << "controlador_de_vuelo_" << t.num_vuelo;
	strcpy(processName, sstr.str().c_str());

   char *args_controlador_vuelo[] = { (char*) processName, (char*)dir, (char*) buffer_num_vuelo, NULL };
	Process controlador_vuelo1("controlador_de_vuelo", args_controlador_vuelo);
}
