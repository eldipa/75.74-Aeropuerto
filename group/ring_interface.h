#ifndef RING_INTERFACE_H_
#define RING_INTERFACE_H_

#include "messagequeue.h"

#define RING_MAX_MESSAGE_SIZE  8192

class RingInterface {
   private:
      MessageQueue inbound;
      MessageQueue outbound;

   public:
      /*
       * This will create an interface with the underlying communication system.
       * 
       * The absolute_path and proj_id are the same parameters used to start that system.
       * And because that, proj_id must be a number between 0 and 127 (inclusive).
       *
       * (*) You can create any number of instances of this class BUT each instance MUST
       *     be connected to one UNIQUE node.
       *     So, if you have one node in one machine, you can instanciate only one object.
       *
       *     This is because the pull method will remove the messages from the internal queue.
       *
       * (*) IF YOU APPLICATION SUPPORT it, you CAN create any number of instance for the SAME node.
       *     In that case, your instances and the communication system will form a producer-consumer.
       *
       * Please, see the documentation of MessageQueue and the documentation of the communication system.
       *
       * */
      RingInterface(const char *absolute_path, char proj_id);

      /* 
       * Push a message to the next node. The communication system will handle this and will deliver the message
       * for you.
       * 'size_msg' is the size in bytes of the message.
       *
       * The other method, will pull a new message from the communication system and will store it in the 'msg'
       * buffer. The buffer must have the memory reserved before you call 'pull' and the size of the buffer must be
       * defined in 'max_size_msg'.
       * Note that this is an upper bound. If the message has less bytes, only those bytes will be readed (and that size
       * will be returned).
       * If the message is bigger, an exception will be raised.
       *
       * See the documentation of MessageQueue.
       * Also, take note that the max size is defined in RING_MAX_MESSAGE_SIZE.
       * */
      void push(const char *msg, size_t size_msg);
      ssize_t pull(char *msg, size_t max_size_msg);
};

#endif
