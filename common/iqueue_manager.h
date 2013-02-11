#ifndef IQUEUE_MANAGER_H_
#define IQUEUE_MANAGER_H_

#include "yasper.h"
class IQueueManager {
public:
   virtual yasper::ptr<IMessageQueue> get_queue(const char* queue_id, char proj_id, bool create =  false) = 0;

};
#endif
