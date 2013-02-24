#include "process.h"
#include "log.h"
#include "iniparser.h"

#include "constants.h"
#include "genericerror.h"
#include "daemon.h"
#include <signal.h>

static char working_dir[MAX_PATH_SIZE];
static char log_path[MAX_PATH_SIZE];
static char persist_path[MAX_PATH_SIZE];
static char port[6];

static char *args_message_broker[] = { (char*) "message_broker_server", working_dir, port, log_path, persist_path, NULL };


int main(int argc, char** argv) {
   try {
      std::string config_file( (argc>1)?argv[1]:DEFAULT_CONFIG_FILE );

      dictionary * ini = NULL;

      if ( (ini=iniparser_load(config_file.c_str())) == NULL ) {
         Log::crit("cannot parse the config file: %s\n", config_file.c_str());
         throw GenericError("cannot parse the config file %s", config_file.c_str());
      }

      int use_ipc = iniparser_getboolean(ini, "MESSAGE_BROKER:use_ipc", -1);

      if(use_ipc) {
         Log::notice("No se lanzan los message_broker, use_ipc=true en archivo de configuracion %s", config_file.c_str());
         return 0;
      }

      Log::notice("Launch message_broker_server, read configuration from %s", config_file.c_str());

      strcpy(working_dir, iniparser_getstring(ini, "MESSAGE_BROKER:working_dir", NULL));
      strcpy(port, iniparser_getstring(ini, "MESSAGE_BROKER:port", NULL));
      strcpy(log_path, iniparser_getstring(ini, "MESSAGE_BROKER:log_path", NULL));
      strcpy(persist_path, iniparser_getstring(ini, "MESSAGE_BROKER:persist_path", NULL));

      Process process_server("./../local_broker/message_broker_server", args_message_broker);
      
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
