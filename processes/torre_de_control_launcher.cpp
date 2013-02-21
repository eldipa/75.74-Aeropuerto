#include "process.h"
#include "log.h"
#include "iniparser.h"

#include "constants.h"
#include "genericerror.h"
#include "daemon.h"
#include <signal.h>
#include <unistd.h>
#include <string.h>
static char working_dir[MAX_PATH_SIZE];

static char *args_torre_de_control[] = { (char*) "message_broker_server", working_dir, NULL };


int main(int argc, char** argv) {
   try {
      std::string config_file( (argc>1)?argv[1]:DEFAULT_CONFIG_FILE );
      
      getcwd(working_dir, MAX_PATH_SIZE);
      strcpy(working_dir+strlen(working_dir), "/");
      strcpy(working_dir+strlen(working_dir), config_file.c_str());

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
