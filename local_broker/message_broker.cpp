#include "message_broker.h"

#include "oserror.h"
#include "message_broker_error.h"

#include "messagequeue.h"
#include "log.h"


MessageBroker::MessageBroker(const char* absolute_path, bool create) : absolute_path(absolute_path) {
   create = create;

   for(int i=0; i<255; i++)
      memset(proj_id_to_queue_id[i], 0x00, 255);
}

MessageBroker::MessageBroker(const char* absolute_path) : absolute_path(absolute_path) {
}

void  MessageBroker::create_queue(const char* queue_id) {
   try {
      char proj_id = allocate_queue_id(queue_id);
      MessageQueue queue( absolute_path.c_str(), (char)proj_id, 0664, true, false);
   } catch(OSError error) {
      throw MessageBrokerError( "OSError: Error al crear la cola %s", error.what());
   }
}

void MessageBroker::destroy_queue(const char* queue_id) {
   try {
      char proj_id = get_proj_id(queue_id);
      MessageQueue queue( absolute_path.c_str(), (char)proj_id, 0664, false, true);
      deallocate_queue_id(queue_id);
   } catch(OSError error) {
      throw MessageBrokerError( "OSError: Error al destruir la cola %s", error.what());
   }
}

void MessageBroker::push(const char* queue_id, const void *msg, size_t size_txt) {

   try {
      char proj_id = get_proj_id(queue_id);
      MessageQueue queue( absolute_path.c_str(), (char)proj_id, 0664, false, false);
      queue.push(msg, size_txt);
   } catch(OSError error) {
      throw MessageBrokerError( "OSError: Error al hacer push() en la cola %s", error.what());
   }
}

ssize_t MessageBroker::pull(const char* queue_id, void *msg, size_t max_size_txt, long type) {
   ssize_t result = -1;
   try {
      char proj_id = get_proj_id(queue_id);
      MessageQueue queue( absolute_path.c_str(), (char)proj_id, 0664, false, false);
      result = queue.pull(msg, max_size_txt, type);
   } catch(OSError error) {
      throw MessageBrokerError( "OSError: Error al hacer pull() en la cola %s", error.what());
   }
   return result;
}

   
char MessageBroker::allocate_queue_id(std::string queue_id) {
   int pos = 0;
   int proj_id = -1;

   while( (proj_id==-1) && (pos<255) ) {
      if( strlen(proj_id_to_queue_id[pos]) == 0 )
         proj_id = (char)pos;
      pos++;
   }

   if( proj_id == -1 )
      throw MessageBrokerError("No hay colas disponibles  en el broker no puedo crear cola con id %s", queue_id.c_str());
   else
      strcpy( proj_id_to_queue_id[(int)proj_id], queue_id.c_str());

   return (char)proj_id;
}

void MessageBroker::deallocate_queue_id(std::string queue_id) {
   try {
      int pos = (int)get_proj_id(queue_id);
      memset(proj_id_to_queue_id[pos], 0x00, 255);
   } catch(MessageBrokerError error) {
      throw error;
   }
}

char MessageBroker::get_proj_id(std::string queue_id) {
   int proj_id = -1;
   int pos = 0;
   
   while( (proj_id==-1) && (pos<255) ) {
      if( strcmp(proj_id_to_queue_id[pos], queue_id.c_str()) == 0 )
         proj_id = pos;
      pos++;
   }

   if(proj_id == -1)
      Log::info("No existe queue en el broker con id %s", queue_id.c_str());

   return (char)proj_id;
}


MessageBroker::~MessageBroker() {
   for(int i=0; i<255; i++) {
      if(proj_id_to_queue_id[i][0] != 0x00) {
         MessageQueue queue( absolute_path.c_str(), (char)i, 0664, false, true);
         memset(proj_id_to_queue_id[i], 0x00, 255);
      }
   }
}

typedef struct t_msg {
   long mtype;
   char data[6];
} tMessage;

int main() {
   
   tMessage msg;
   msg.mtype = 5;
   strcpy(msg.data, "hello");
   tMessage response;

   MessageBroker broker("/tmp/carpeta_keys_ftok/message_broker.lck", true);

   broker.create_queue("cola1");
   broker.push( "cola1", &msg, sizeof(tMessage)-sizeof(long) );

   broker.pull("cola1", &response, sizeof(tMessage), 0);
   std::cout << response.data;
}
