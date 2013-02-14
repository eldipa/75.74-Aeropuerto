#ifndef RING_INTERFACE_H_
#define RING_INTERFACE_H_

#include "messagequeue.h"

#define RING_MAX_MESSAGE_SIZE  8192

class RingInterface {
   private:
      MessageQueue inbound;
      MessageQueue outbound;

   public:
      RingInterface(const char *absolute_path, char proj_id);

      void push(const char *msg, size_t size_msg);
      ssize_t pull(char *msg, size_t max_size_msg);
};

#endif
