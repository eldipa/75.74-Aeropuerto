#ifndef _BROKER_MESSAGE_QUEUE_H_
#define _BROKER_MESSAGE_QUEUE_H_

#include "imessagequeue.h"
#include "message_broker.h"

class BrokerMessageQueue : public IMessageQueue {
private:
   std::string id;
   bool is_owner;
   yasper::ptr<MessageBroker> broker;

public:
   BrokerMessageQueue(const char* queue_id, char proj_id, bool create, yasper::ptr<MessageBroker> broker) :
      is_owner(create), broker(broker) {

      id =  queue_id;
      id += (proj_id+1);

      if(is_owner)
         broker->create_queue(id.c_str());
   }

   void push(const void *msg, size_t size_txt) {
      broker->push(id.c_str(), msg, size_txt);
   }

   ssize_t pull(void *msg, size_t max_size_txt, long type = 0) {
      return broker->pull(id.c_str(), msg, max_size_txt, type);
   }

   virtual ~BrokerMessageQueue() {
      if(is_owner) {
         try {
            broker->destroy_queue(id.c_str());
         } catch (std::exception error) {
            Log::crit("Error al destruir queue_id:%s", id.c_str());
         }
      }
   }
};

#endif
