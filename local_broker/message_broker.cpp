#include "message_broker.h"

#include "api_constants.h"
#include "oserror.h"
#include "message_broker_error.h"

#include "messagequeue.h"
#include "log.h"


MessageBroker::MessageBroker(const char* directorio_de_trabajo, bool create) : 
   queues_path( std::string(directorio_de_trabajo).append(PATH_MESSAGE_BROKER_QUEUES) ),
   is_owner(true),
   queue_ids( std::string(directorio_de_trabajo).append(PATH_MESSAGE_BROKER).c_str(), 0, 0664, true) {
   
   create = create;
   for(int i=0; i<255; i++)
      memset(queue_ids->proj_id_to_queue_id[i], 0x00, 255);
}

MessageBroker::MessageBroker(const char* directorio_de_trabajo) : 
   queues_path( std::string(directorio_de_trabajo).append(PATH_MESSAGE_BROKER_QUEUES) ),
   is_owner(false),
   queue_ids( std::string(directorio_de_trabajo).append(PATH_MESSAGE_BROKER).c_str(), 0, 0664) {

}

void  MessageBroker::create_queue(const char* queue_id) {
   try {
      char proj_id = allocate_queue_id(queue_id);
      
      Log::info( "broker_create id='%s' proj_id='%c' (%d)", queue_id, proj_id, (int)proj_id);
      MessageQueue queue( queues_path.c_str(), (char)proj_id, 0664, true, false);
   } catch(OSError error) {
      throw MessageBrokerError( "OSError: Error al crear la cola %s", error.what());
   }
}

void MessageBroker::destroy_queue(const char* queue_id) {
   try {
      char proj_id = get_proj_id(queue_id);

      Log::info( "broker_destroy queue_id='%s' local_proj_id='%c' (%d)", queue_id, proj_id, (int)proj_id);
      MessageQueue queue( queues_path.c_str(), (char)proj_id, 0664, false, true);
      deallocate_queue_id(queue_id);
   } catch(OSError error) {
      throw MessageBrokerError( "OSError: Error al destruir la cola %s", error.what());
   }
}

void MessageBroker::push(const char* queue_id, const void *msg, size_t size_txt) {

   try {
      char proj_id = get_proj_id(queue_id);

      Log::info( "broker_push queue_id='%s' local_proj_id='%c' (%d)", queue_id, proj_id, (int)proj_id);

      MessageQueue queue( queues_path.c_str(), (char)proj_id, 0664, false, false);
      queue.push(msg, size_txt);
   } catch(OSError error) {
      throw MessageBrokerError( "OSError: Error al hacer push() en la cola %s", error.what());
   }
}

ssize_t MessageBroker::pull(const char* queue_id, void *msg, size_t max_size_txt, long type) {
   ssize_t result = -1;
   try {
      char proj_id = get_proj_id(queue_id);
      MessageQueue queue( queues_path.c_str(), (char)proj_id, 0664, false, false);

      Log::info( "broker_pull queue_id='%s' local_proj_id='%c' (%d)", queue_id, proj_id, (int)proj_id);
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
      if( strlen(queue_ids->proj_id_to_queue_id[pos]) == 0 )
         proj_id = (char)pos;
      pos++;
   }

   if( proj_id == -1 )
      throw MessageBrokerError("No hay colas disponibles  en el broker no puedo crear cola con id %s", queue_id.c_str());
   else
      strcpy( queue_ids->proj_id_to_queue_id[(int)proj_id], queue_id.c_str());

   return (char)proj_id;
}

void MessageBroker::deallocate_queue_id(std::string queue_id) {
   try {
      int pos = (int)get_proj_id(queue_id);
      memset(queue_ids->proj_id_to_queue_id[pos], 0x00, 255);
   } catch(MessageBrokerError error) {
      throw error;
   }
}

char MessageBroker::get_proj_id(std::string queue_id) {
   int proj_id = -1;
   int pos = 0;
   
   while( (proj_id==-1) && (pos<255) ) {
      if( strcmp(queue_ids->proj_id_to_queue_id[pos], queue_id.c_str()) == 0 )
         proj_id = pos;
      pos++;
   }

   if(proj_id == -1)
      Log::info("No existe queue en el broker con id %s", queue_id.c_str());

   return (char)proj_id;
}


MessageBroker::~MessageBroker() {
   /*
   if(is_owner) {
      for(int i=0; i<255; i++) {
         if(queue_ids->proj_id_to_queue_id[i][0] != 0x00) {
            MessageQueue queue( queues_path.c_str(), (char)i, 0664, false, true);
            memset(queue_ids->proj_id_to_queue_id[i], 0x00, 255);
         }
      }
      }*/
}
