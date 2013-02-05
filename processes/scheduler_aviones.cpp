#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "constants.h"
#include "log.h"

#include "process.h"
#include <list>

typedef struct t_task_avion {
   int id_avion;
   int time;
} tTaskAvion;

bool compare_time (tTaskAvion t1, tTaskAvion t2) {
   return ( t1.time < t2.time );
}


void lanzar_avion(tTaskAvion, const char* dir);
void cargar_aviones( std::list<tTaskAvion>& );

int main(int argc, char *argv[]) try {
   if (argc < 1) {
      Log::crit("Insuficientes parametros para scheduler_aviones, se esperaba (directorio_de_trabajo)\n");
      return (1);
   }
   
   Log::info("Comenzando scheduler_aviones a lanzar aviones");
      
   int time = 0;
   std::list<tTaskAvion> aviones;
   cargar_aviones(aviones);
   aviones.sort(compare_time);
   
   while( !aviones.empty() ) {

      while( (!aviones.empty()) && (aviones.front().time <= time ) ){
         lanzar_avion(aviones.front(), argv[1]);
         aviones.pop_front();
      }
      sleep(1);
      time += 1;
   }

   Log::info("Fin scheduler, no hay mas aviones");
} catch(const std::exception &e) {
   Log::crit("%s", e.what());
} catch(...) {
   Log::crit("Critical error. Unknow exception at the end of the 'main' function.");
}

void lanzar_avion(tTaskAvion task, const char* directorio_de_trabajo) {
   char dir[255];
   char processName[255];
   char buffer_id_avion[6];

   strcpy(dir, directorio_de_trabajo);

   std::stringstream avion;
   avion << task.id_avion;
   strcpy(buffer_id_avion, avion.str().c_str());

   std::stringstream strProcessName;
   strProcessName << "avion_" << task.id_avion;
   strcpy(processName, strProcessName.str().c_str());
   
   Log::info("lanzando proceso avion %s %s %s", processName, dir, buffer_id_avion);

   char *args_avion[] = { (char*) processName, (char*)dir, (char*) buffer_id_avion, NULL };   
	Process generator("avion", args_avion);
}

void cargar_aviones(std::list<tTaskAvion>& list) {
   char primera_linea[255];
	FILE * file_aviones;
   tTaskAvion t;
   int num_vuelo = 0;

	file_aviones = fopen("./entrada/aviones.csv", "rt");
	fscanf(file_aviones, "%[^\n]s\n", primera_linea);

	while (fscanf(file_aviones, "%d:%d:%d\n",&t.time, &t.id_avion, &num_vuelo) != EOF) {
      Log::debug("push avion %d", t.id_avion);
      list.push_front(t);
	}

	fclose(file_aviones);

}
