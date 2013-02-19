#include "message_broker_stub.h"

#include <sstream>
#include "oserror.h"
#include "log.h"

#include "message_broker_server.h"


MessageBrokerStub::MessageBrokerStub(const char* ip, const char* port) :
   ip(ip), port(port), sock(true) {

   try {
      sock.destination(ip, port);
   } catch(std::exception error) {
      throw MessageBrokerError("Can't connect to message broker at %s:%s ", ip, port);
   }
}

void MessageBrokerStub::create_queue(const char* queue_id) {

   BrokerRequest request;
   BrokerResponse response;

   request.mtypebroker = BROKER_CREATE;
   strcpy(request.queue_id, queue_id);   
   strcpy(request.sender_procname, get_proc_name().c_str() );
 
   try {
      MessageBrokerServer::sendall(sock, (char*)&request, sizeof(BrokerRequest));
      MessageBrokerServer::receiveall(sock, (char*)&response, sizeof(BrokerResponse));
   } catch(OSError error) {
      throw MessageBrokerError("Network error, cause: %s", error.what());
   }

   if(response.mtypebroker == BROKER_ERROR)
      throw MessageBrokerError("Error al crear la cola en el broker remoto");
}

void MessageBrokerStub::destroy_queue(const char* queue_id) {

   Log::debug("MessageBrokerStub: triying to destroy the queue_id '%s'", queue_id);

   BrokerRequest request;
   BrokerResponse response;

   request.mtypebroker = BROKER_DESTROY;
   strcpy(request.queue_id, queue_id);   
   strcpy(request.sender_procname, get_proc_name().c_str() );

   try {
      MessageBrokerServer::sendall(sock, (char*)&request, sizeof(BrokerRequest));
      MessageBrokerServer::receiveall(sock, (char*)&response, sizeof(BrokerResponse));
      Log::debug("MessageBrokerStub: queue destroy ok '%s'", queue_id);
   } catch(std::exception error) {
      throw MessageBrokerError("Network error, cause: %s", error.what());
   }

   if(response.mtypebroker == BROKER_ERROR)
      throw MessageBrokerError("Error al destruir la cola en el broker remoto");
}

void MessageBrokerStub::push(const char* queue_id, const void *msg, size_t size_txt) {

   Log::debug("MessageBrokerStub: triying to send msg of size %d to queue_id '%s'", size_txt, queue_id);

   if(size_txt+sizeof(long)>MAX_MSG_SIZE)
      throw MessageBrokerError("Message for %s too long size=%d, max_size=%d", queue_id, size_txt, MAX_MSG_SIZE);

   BrokerRequest request;
   BrokerResponse response;

   request.mtypebroker = BROKER_PUSH;

   strcpy(request.queue_id, queue_id);   
   request.size_txt = size_txt;
   memcpy(request.msg, msg, size_txt+sizeof(long));
   strcpy(request.sender_procname, get_proc_name().c_str() );
   
   try {
      MessageBrokerServer::sendall(sock, (char*)&request, sizeof(BrokerRequest));
      MessageBrokerServer::receiveall(sock, (char*)&response, sizeof(BrokerResponse));
   } catch(OSError error) {
      throw MessageBrokerError("Network error, cause: %s", error.what());
   }

   if(response.mtypebroker == BROKER_ERROR)
      throw MessageBrokerError("Error al hacer push en la cola en el broker remoto");
}

ssize_t MessageBrokerStub::pull(const char* queue_id, void *msg, size_t max_size_txt, long type) {

   Log::debug("MessageBrokerStub: triying to pull of the queue_id '%s' with type %d", queue_id, type);

   BrokerRequest request;
   BrokerResponse response;

   request.mtypebroker = BROKER_PULL;
   strcpy(request.queue_id, queue_id);   
   request.max_size_txt = max_size_txt;
   request.type = type;
   strcpy(request.sender_procname, get_proc_name().c_str() );

   if(max_size_txt+sizeof(long)>MAX_MSG_SIZE)
      throw MessageBrokerError("Message for %s too long size=%d, max_size=%d", queue_id, max_size_txt, MAX_MSG_SIZE);

   try {
      MessageBrokerServer::sendall(sock, (char*)&request, sizeof(BrokerRequest));
      MessageBrokerServer::receiveall(sock, (char*)&response, sizeof(BrokerResponse));
      Log::debug("MessageBrokerStub: Receive from queue_id %s", queue_id);
   } catch(OSError error) {
      throw MessageBrokerError("Network error, cause: %s", error.what());
   }

   if(response.mtypebroker == BROKER_ERROR) {
      throw MessageBrokerError("Error al hacer push en la cola en el broker remoto");
   } else {
      memcpy(msg, response.msg, max_size_txt+sizeof(long));
   }
   return response.msg_size;
}
 
std::string MessageBrokerStub::get_proc_name() {
    std::ifstream comm("/proc/self/comm");
    std::string name;
    getline(comm, name);
    return name;
 }

MessageBrokerStub::~MessageBrokerStub() {
}

