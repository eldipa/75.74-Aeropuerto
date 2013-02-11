#ifndef BROKER_QUEUE_MANAGER_H_
#define BROKER_QUEUE_MANAGER_H_

#include "iqueue_manager.h"

#include "message_broker.h"
#include "messagequeue.h"
#include "yasper.h"
#include <string>
#include "log.h"

class BrokerQueueManager : public IQueueManager {
private:

   class BrokerMessageQueue : public IMessageQueue {
   public:
      BrokerMessageQueue(const char *queue_id,  yasper::ptr<MessageBroker> broker, bool create = false) : 
         is_owner(create), queue_id(queue_id), broker(broker) {
         if(is_owner)
            broker->create_queue(queue_id);
      }


      void push(const void *msg, size_t size_txt) {
         broker->push(queue_id.c_str(), msg, size_txt);
      }

      ssize_t pull(void *msg, size_t max_size_txt, long type = 0) {
         return broker->pull(queue_id.c_str(), msg, max_size_txt, type);
      }

      ~BrokerMessageQueue() {
         if(is_owner)
            broker->destroy_queue(queue_id.c_str());
      }

   private:
      bool is_owner;
      std::string queue_id;
      yasper::ptr<MessageBroker> broker;
   };

public:

   BrokerQueueManager(const char* path_to_locks) : abs_path(path_to_locks) {
      abs_path.append(PATH_MESSAGE_BROKER);
   }

   virtual yasper::ptr<IMessageQueue> get_queue(const char* queue_id, char proj_id, bool create =  false) {

      std::string queue_broker_id(queue_id);
      queue_broker_id = queue_broker_id + "_" + proj_id;

      return new BrokerMessageQueue(queue_broker_id.c_str(), new MessageBroker(abs_path.c_str()), create);
   }

   virtual ~BrokerQueueManager() {
   }

private:
   std::string abs_path;


};
#endif
