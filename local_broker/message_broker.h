#ifndef MESSAGE_BROKER_H_
#define MESSAGE_BROKER_H_

#include <iostream>
#include <string.h>
#include <stdexcept>


class MessageBroker {

public:

	MessageBroker(const char* absolute_path, bool create);
	MessageBroker(const char* absolute_path);

   void create_queue(const char* queue_id);
   void destroy_queue(const char* queue_id);

   void push(const char* queue_id, const void *msg, size_t size_txt);
   ssize_t pull(const char* queue_id, void *msg, size_t max_size_txt, long type = 0);

	virtual ~MessageBroker();

private:

   std::string absolute_path;
   char  proj_id_to_queue_id [255][255];

   
   char allocate_queue_id(std::string queue_id);
   void deallocate_queue_id(std::string queue_id);
   char get_proj_id(std::string queue_id);

};


#endif /* MESSAGEBROKER_H_ */
 
