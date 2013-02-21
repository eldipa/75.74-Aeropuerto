#ifndef MESSAGE_BROKER_SERVER_H_ 
#define MESSAGE_BROKER_SERVER_H_ 

#include "server.h"
#include "imessage_broker.h"

#include "yasper.h"
#include <string>
#include "log.h"

#include <iostream>
#include <fstream>

class MessageBrokerServer : public Server {
public:
   MessageBrokerServer(yasper::ptr<IMessageBroker> broker, const char* service, 
                       const char* path_log_file, const char* path_persist_file, bool load_from_file );
   virtual void client_handler(Socket&);
   virtual ~MessageBrokerServer();

   static bool sendall(Socket& sock, const char *buf, size_t data_len) {
      size_t total, enviados;
      enviados = 0;
      total = 0;
      
      do {
         enviados = sock.sendsome(&buf[total], data_len-total);
         total += enviados;
      } while( (enviados>0) && (total<data_len) );
      
      return (total >= data_len);
   }

   static bool receiveall(Socket& sock, char *buf, size_t data_len) {
      size_t total, recibidos;
      recibidos = 0;
      total = 0;
      
      do {
         recibidos = sock.receivesome(&buf[total], data_len-total);
         total += recibidos;
      } while( (recibidos>0) && (total<data_len) );
      
      return (total >= data_len);
   }


private:
   
   yasper::ptr<IMessageBroker> broker;
   std::string service;
   
   std::ofstream log_file;
   std::ofstream persist_file;

};

#endif
