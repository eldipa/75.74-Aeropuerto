#ifndef IMESSAGE_BROKER_H_ 
#define IMESSAGE_BROKER_H_ 

#include "notimplementederror.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>

class IMessageBroker {
public:
   virtual void create_queue(const char* queue_id) = 0;
   virtual void destroy_queue(const char* queue_id) = 0;

   virtual void push(const char* queue_id, const void *msg, size_t size_txt) = 0;
   virtual ssize_t pull(const char* queue_id, void *msg, size_t max_size_txt, long type = 0) = 0;

   virtual bool exist_queue(const char* queue_id) {
      throw NotImplementedError("imessage_broker->exist_queue(queue_id=%s)", queue_id);
   }

   virtual ~IMessageBroker() {
   }
};

#endif
