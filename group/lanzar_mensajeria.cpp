
#include "valueerror.h"
#include <sstream>
#include <cstdlib>
#include "lanzar_mensajeria.h"
#include <iostream>

void lanzar_mensajeria(const char* absolute_path, char proj_id, char group_id,
      const char* localhost_name, const char* network_name) {

   if(proj_id < 0)
      throw ValueError("The project id is out of range. It must be in between 0 and 127 (inclusive) but the id is %i (path: %s)", proj_id, absolute_path);

   std::ostringstream cmd;

   cmd << "python inbound.py" <<
      " " << absolute_path <<
      " " << int(proj_id) <<
      " " << int(group_id) <<
      " " << localhost_name <<
      " " << network_name <<
      " " << "&";

   system(cmd.str().c_str());
  // std::cout << cmd.str() << std::endl;
}

void lanzar_beacon_svc(const char* localhost_name){
	 std::ostringstream cmd;

	   cmd << "python beacon_service.py" <<
	      " " << localhost_name <<
	      " " << "&";

	   system(cmd.str().c_str());
	   //std::cout << cmd.str() << std::endl;
}

void bajar_grupos(){
	 std::ostringstream cmd;

	   cmd << "pkill python &";

	   system(cmd.str().c_str());
	   //std::cout << cmd.str() << std::endl;
}

