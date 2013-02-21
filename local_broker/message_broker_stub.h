#ifndef MESSAGE_BROKER_STUB_H_
#define MESSAGE_BROKER_STUB_H_

#include <iostream>
#include <string.h>
#include <stdexcept>
#include "imessage_broker.h"
#include "sharedobject.h"
#include "notimplementederror.h"

#include "socket.h"
#include "message_broker_error.h"

#include <iostream>
#include <fstream>
#include <sstream>

#define MAX_MSG_SIZE 16384

#define BROKER_CREATE  0
#define BROKER_PUSH    1
#define BROKER_PULL    2
#define BROKER_DESTROY 3

#define BROKER_ERROR 4
#define BROKER_OK    5

class BrokerRequest {
public:
   int mtypebroker;
   char sender_procname[255];
   char queue_id[255];
   size_t size_txt;
   size_t max_size_txt;
   long type;
   char msg[MAX_MSG_SIZE];

   friend std::ostream& operator<<(std::ostream& out,  BrokerRequest& request) {

      switch( request.mtypebroker ) {
      case BROKER_CREATE:
         request.size_txt = 0;
         out << "CREATE :";
         break;
      case BROKER_PUSH:
         out << "PUSH   :";
         break;
      case BROKER_PULL:
         request.size_txt = 0;
         out << "PULL   :";
         break;
      case BROKER_DESTROY:
         request.size_txt = 0;
         out << "DESTROY:";
         break;
      }

      std::string qname(request.queue_id, strlen(request.queue_id)-1);
      char proj_id = request.queue_id[qname.length()];

      out << " from " << request.sender_procname << " to " << qname << "("  << ((int)proj_id)-1 << ")";
      out <<  " type:" << request.type;

      out << " msg_size:" << request.size_txt;
      // request.dump_data(std::cout, request.msg, request.size_txt);
      // out << std::endl << "</msg>";

      // std::cout << std::endl;

      return out;
   }

   void dump_data(std::ostream& out, char* ptr, int buflen) {
      out << "";
      ptr = ptr;
      buflen = buflen;
   }


};

class BrokerResponse {
public:
   int mtypebroker;
   char msg[MAX_MSG_SIZE];
   ssize_t msg_size;
};

class MessageBrokerStub : public IMessageBroker {

public:

	MessageBrokerStub(const char* ip, const char* port);

   void create_queue(const char* queue_id);
   void destroy_queue(const char* queue_id);

   void push(const char* queue_id, const void *msg, size_t size_txt);
   ssize_t pull(const char* queue_id, void *msg, size_t max_size_txt, long type = 0);


	virtual ~MessageBrokerStub();

private:
   std::string ip;
   std::string port;
   Socket sock;

   std::string get_proc_name();
};


#endif
 
