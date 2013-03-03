#include "process.h"
#include "log.h"
#include "iniparser.h"

#include "constants.h"
#include "genericerror.h"
#include "daemon.h"
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include "api_torre_de_control.h"
#include "api_configuracion.h"
#include "api_tractor.h"

static char working_dir[MAX_PATH_SIZE];

static char *args_torre_de_control[] = { (char*) "torre_de_control_svc", working_dir, NULL };


int main(int argc, char** argv) {
   try {
      std::string config_file( (argc>1)?argv[1]:DEFAULT_CONFIG_FILE );
      strcpy(working_dir, config_file.c_str());

      ApiTorreDeControl api_torre( ApiConfiguracion::get_wkdir(config_file.c_str()).c_str(), config_file.c_str(), true);
      ApiTractor api_tractor( ApiConfiguracion::get_wkdir(config_file.c_str()).c_str(), config_file.c_str(), 1, true);

      for (int i = 0; i < ApiConfiguracion::get_contenedores(config_file.c_str()); i++) {
		 	api_torre.liberar_contenedor();
      }

      for (int i = 1; i <= ApiConfiguracion::get_zonas(config_file.c_str()); i++) {
		 	api_torre.liberar_zona(i);
      }

      Process process_server("./../rpc/torre_de_control_svc", args_torre_de_control);
      
      Log::notice("Done, waiting for a SIGINT or a SIGTERM signal.");
      wait_signal();

      Log::notice("Signal recieved. Shutdown...");
      process_server.send_signal(SIGTERM);
      process_server.send_signal(SIGKILL);


   } catch (const std::exception &e) {
      Log::crit("%s", e.what());
   }
   catch (...) {
      Log::crit("Critical error. Unknow exception at the end of the 'main' function.");
   }

   return 0;
}
