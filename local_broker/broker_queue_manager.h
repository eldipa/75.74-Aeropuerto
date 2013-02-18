#ifndef BROKER_QUEUE_MANAGER_H_
#define BROKER_QUEUE_MANAGER_H_

#include "iqueue_manager.h"

#include "message_broker.h"
#include "brokermessagequeue.h"
#include "message_broker_stub.h"

#include <string>
#include <sstream>

class BrokerQueueManager : public IQueueManager {

public:

   BrokerQueueManager(yasper::ptr<IMessageBroker> broker) : broker(broker) {
   }

   virtual yasper::ptr<IMessageQueue> get_queue(const char* queue_id, char proj_id, bool create =  false) {
      return new BrokerMessageQueue(queue_id, proj_id, create, broker);
   }

   virtual ~BrokerQueueManager() {
   }

private:
   yasper::ptr<IMessageBroker> broker;
};

/*
class NetworkQueueManager : public IQueueManager{
public:
   NetworkQueueManager(const char* ip, int port = 7000) : ip(ip) {
      std::stringstream sstr;sstr << port;
      service = sstr.str();
   }

   virtual yasper::ptr<IMessageQueue> get_queue(const char* queue_id, char proj_id, bool create =  false) {
      yasper::ptr<IMessageBroker>  broker = new MessageBrokerStub(ip.c_str(), service.c_str());
      return new BrokerMessageQueue(queue_id, proj_id, create, broker);
   }

   virtual ~NetworkQueueManager() {
   }
private:

   std::string ip;
   std::string service;
};
*/
#endif
