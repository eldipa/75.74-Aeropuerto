#ifndef BROKER_QUEUE_MANAGER_H_
#define BROKER_QUEUE_MANAGER_H_

#include "iqueue_manager.h"

#include "message_broker.h"
#include "brokermessagequeue.h"

class BrokerQueueManager : public IQueueManager {

public:

   BrokerQueueManager(const char* path_to_locks) : abs_path(path_to_locks) {
   }

   virtual yasper::ptr<IMessageQueue> get_queue(const char* queue_id, char proj_id, bool create =  false) {
      queue_id = queue_id;
      proj_id = proj_id;
      create = create;
      return new BrokerMessageQueue(queue_id, proj_id, create, new MessageBroker(abs_path.c_str()));
      // std::string queue_broker_id(queue_id);
      // queue_broker_id = queue_broker_id + "_" + proj_id;

      // return new BrokerMessageQueue(queue_broker_id.c_str(), new MessageBroker(abs_path.c_str()), create);
   }

   virtual ~BrokerQueueManager() {
   }

private:
   std::string abs_path;


};
#endif
