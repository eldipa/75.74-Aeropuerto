#ifndef API_DESPACHANTE_H_
#define API_DESPACHANTE_H_

#include "api_constants.h"
#include "equipaje.h"
#include "cintas.h"
#include "rfid.h"

#include "sharedobject.h"
#include "semaphoreset.h"
#include "mutex.h"

#include <stdexcept>

class AsignacionZona;
class ZonasAsignadas;

class ApiDespachante {
private:
   //SemaphoreSet sem_set;
  // Mutex mutex_asignaciones;
   //SharedObject<ZonasAsignadas> asignaciones;

	CintaCentral cinta;
	int numero_despachante;

	Equipaje ultimo_equipaje_leido;
	bool saco_elemento;
public:
	ApiDespachante(const char* directorio_de_trabajo, const char* config_file, int numero_despachante);
	virtual ~ApiDespachante();

	Rfid leer_proximo_equipaje();
	Equipaje extraer_equipaje();
	void avanzar_cinta();

   void asignar_vuelo(int zona, int vuelo);
   void desasignar_vuelo(int zona);
   int get_zona(int num_vuelo);
   
   static const int cant_ipcs = 2;
};


class AsignacionZona {
private:
   int num_zona;
   int num_vuelo;
public:
   AsignacionZona():num_zona(0), num_vuelo(0) {
   }

   AsignacionZona(int zona, int vuelo):num_zona(zona), num_vuelo(vuelo) {
   }

   int zona() const { return num_zona; }
   int vuelo() const { return num_vuelo; }
   int get_zona(int num_vuelo);
};

class ZonasAsignadas {
public:
   ZonasAsignadas() : cant_zonas_asignadas(0) {
   }

   void asignar_vuelo(int zona, int vuelo) {
      if(cant_zonas_asignadas == MAX_ZONAS)
         throw std::runtime_error("Maximo de zonas asignadas");

      zonas[cant_zonas_asignadas] = AsignacionZona(zona, vuelo);
      cant_zonas_asignadas++;
      Log::info("ApiDespachante zona asignada zona=%d vuelo=%d cant=%d", zonas[cant_zonas_asignadas-1].zona(),
                zonas[cant_zonas_asignadas-1].vuelo(), cant_zonas_asignadas);
   }

   void desasignar_vuelo(int vuelo) {
      int eliminados = 0;
      for (int i=0; i<cant_zonas_asignadas; i++) {
         if(zonas[i].vuelo() == vuelo)
            eliminados++;
         else
            zonas[i-eliminados] = zonas[i];
      }
   }

   int get_zona(int num_vuelo) const {
      int zona = -1;
      int i = 0;
      while( (zona==-1) && (i<cant_zonas_asignadas)) {
         Log::info("ApiDespachante get_zona(num_vuelo=%d): zona=%d vuelo =%d", num_vuelo, zonas[i].zona(), zonas[i].vuelo());
         if(zonas[i].vuelo() == num_vuelo) {
            zona = zonas[i].zona();
         }
         i++;
      }
      return zona;
   }

private:
   int cant_zonas_asignadas;
   AsignacionZona zonas[MAX_ZONAS];
};

#endif
