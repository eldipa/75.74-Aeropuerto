#ifndef IPC_QUEUE_MANAGER_H
#define IPC_QUEUE_MANAGER_H_

#include "iqueue_manager.h"

#include "messagequeue.h"
#include "yasper.h"
#include <string>
#include "log.h"

class IpcQueueManager : public IQueueManager {
public:
   IpcQueueManager(const char* path_to_locks, int permissions = 0664) : 
      path_to_locks(path_to_locks),
      permissions(permissions) {
      
      Log::info( "Initializing IpcQueueManager with path_to_locks %s and permissions %d", path_to_locks, permissions);
   }

   virtual yasper::ptr<IMessageQueue> get_queue(const char* queue_id, char proj_id, bool create =  false) {
      std::string abs_path(path_to_locks);
      abs_path.append(queue_id);
      return new MessageQueue(abs_path.c_str(), proj_id, permissions, create);
   }

private:
   std::string path_to_locks;
   int permissions;
};
#endif
