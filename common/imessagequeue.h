#ifndef _IMESSAGEQUEUE_H_
#define _IMESSAGEQUEUE_H_

class IMessageQueue {
public:
   virtual void push(const void *msg, size_t size_txt) = 0;
   virtual ssize_t pull(void *msg, size_t max_size_txt, long type = 0) = 0;
};

#endif
