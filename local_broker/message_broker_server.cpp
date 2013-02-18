#include "message_broker_server.h"

#include "server.h"
#include "imessage_broker.h"
#include "yasper.h"

#include "message_broker_stub.h"
#include "imessage_broker.h"
#include "message_broker.h"
#include "log.h"
#include "oserror.h"
#include "daemon.h"

#include "api_constants.h"

#include <sys/stat.h>
#include <cerrno>
#include <cstdio>
#include <fcntl.h>

MessageBrokerServer::MessageBrokerServer(yasper::ptr<IMessageBroker> broker,  const char* service)
   : Server(service), broker(broker), service(service) {
}

void MessageBrokerServer::client_handler(Socket& sock) {
   bool continue_comm = true;

   while(continue_comm) {

      BrokerResponse response;
      BrokerRequest request;

      continue_comm = MessageBrokerServer::receiveall(sock, (char*)&request, sizeof(BrokerRequest));

      response.mtypebroker = BROKER_OK;
            
      if( continue_comm ) {

         try {
            
            if(response.mtypebroker == BROKER_OK) {
               switch( request.mtypebroker ) {
               case BROKER_CREATE:
                  broker->create_queue(request.queue_id);
                  break;
               case BROKER_PUSH:
                  broker->push(request.queue_id, request.msg, request.size_txt);
                  break;
               case BROKER_PULL:
                  broker->pull(request.queue_id, response.msg, request.max_size_txt, request.type);
                  break;
               case BROKER_DESTROY:
                  broker->destroy_queue(request.queue_id);
                  break;
               }
            }

            MessageBrokerServer::sendall(sock, (char*)&response, sizeof(BrokerResponse));
               
         } catch(std::exception error) {
            Log::debug("MessageBrokerServer: error %s", error.what());
         }
         
        
      }
         
   }
}


MessageBrokerServer::~MessageBrokerServer() {
}


void crear_archivo_lck(const char *path_to_locks, const char * nombre_archivo);
void crear_archivos_lck(const char *path_to_locks);

int main(int argc, char** argv) {
   try {
      ignore_signals();

      std::string path_to_locks = (argc>1)?argv[1]:"/tmp/carpeta_keys_ftok";
      std::string service = (argc>2)?argv[2]:"9000";

      crear_archivos_lck(path_to_locks.c_str());

      yasper::ptr<IMessageBroker> broker = new MessageBroker(path_to_locks.c_str(), true);
      MessageBrokerServer server(broker, service.c_str());
      server.run();

   } catch (const std::exception &e) {
      Log::crit("%s", e.what());
   } catch (...) {
      Log::crit("Critical error. Unknow exception at the end of the 'main' function.");
   }

   return 0;
}

void crear_archivos_lck(const char *path_to_locks) {

   struct stat buf;
   int result;

   result = stat(path_to_locks, &buf);

   if (result != 0) {
      /* Directory does not exist. EEXIST for race condition */
      if (mkdir(path_to_locks, 0770) != 0 && errno != EEXIST) {
         //THROW OSERROR
      }
   } else if (!S_ISDIR(buf.st_mode)) {
      errno = ENOTDIR;
      //THROW OSERROR
   }

   crear_archivo_lck(path_to_locks, PATH_MESSAGE_BROKER);
   crear_archivo_lck(path_to_locks, PATH_MESSAGE_BROKER_QUEUES);
}

void crear_archivo_lck(const char *path_to_locks, const char * nombre_archivo) {
   char path[300];
   int file;
   struct stat buf;
   
   strcpy(path, path_to_locks);
   strcat(path, nombre_archivo);
   
   if (stat(path, &buf) != 0) {
      file = open(path, O_CREAT | 0664);
      if (file != -1) {
         close(file);
      } else {
         //THROW OSERROR
      }
   }
}
