#include "server.h"
#include "log.h"

#include <stdlib.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/wait.h>
#include "oserror.h"

Server::Server(const char* service) : service(service), continue_listening(true), cant_childs(0) {
}

void Server::run() {
   int pid;

   Log::info("Run server, service:%s", service.c_str());

   try{
      Socket server_socket(true);
      server_socket.source(service);
            
      for(;;) {

         int client_fd = server_socket.listen_fd(10);
         
         if ( (pid=fork()) == 0 ) {
            try {
               Log::info("Receive connection --> Spawn client_handler" );
               Socket client_socket(client_fd);
               this->client_handler(client_socket);
            } catch (std::exception error) {
               Log::crit("Client handler error: %s", error.what());
            }
            exit(0);
            
         } else if( pid>=0 ) {
            cant_childs++;
            close(client_fd);
         }
      }

   } catch (const std::exception &e) {
      Log::crit("ServerError: %s", e.what());
   } catch (...) {
      Log::crit("ServerError: Critical error. Unknow exception at the end of the 'main' function.");
   }
   
   Log::info("Stop server, service:%s", service.c_str());
   
   for(int i = 0; i<cant_childs; i++) {
      int status;
      wait(&status);
   }  

}


Server::~Server() {
}
