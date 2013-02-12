#ifndef MESSAGE_BROKER_H_
#define MESSAGE_BROKER_H_

#include <iostream>
#include <string.h>
#include <stdexcept>

#include "sharedobject.h"

class MessageBrokerData {
public:
   char  proj_id_to_queue_id [255][255];
};

class MessageBroker {

public:

	MessageBroker(const char* directorio_de_trabajo, bool create);
	MessageBroker(const char* directorio_de_trabajo);

   void create_queue(const char* queue_id);
   void destroy_queue(const char* queue_id);

   void push(const char* queue_id, const void *msg, size_t size_txt);
   ssize_t pull(const char* queue_id, void *msg, size_t max_size_txt, long type = 0);

	virtual ~MessageBroker();

private:
   std::string queues_path;
   bool is_owner;
   SharedObject<MessageBrokerData> queue_ids;
   
   char allocate_queue_id(std::string queue_id);
   void deallocate_queue_id(std::string queue_id);
   char get_proj_id(std::string queue_id);

};


#endif /* MESSAGEBROKER_H_ */
 
