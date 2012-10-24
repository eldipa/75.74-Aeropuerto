#ifndef VUELO_H_
#define VUELO_H_

#include <vector>
#include <string>
#include <time.h>
   
class Vuelo {
public:
   int num_vuelo;
   std::string compania;
   std::string origen;
   std::string destino;
   time_t fecha_hora_salida;
   time_t fecha_hora_llegada;
   time_t fecha_hora_checkin;
   int num_zona_asignada;
};

#endif /* VUELO_H_ */
