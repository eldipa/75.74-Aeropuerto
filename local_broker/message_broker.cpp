#include "message_broker.h"

#include "api_constants.h"
#include "oserror.h"
#include "message_broker_error.h"

#include "messagequeue.h"
#include "log.h"


MessageBroker::MessageBroker(const char* directorio_de_trabajo, bool create) : 
   queues_path( std::string(directorio_de_trabajo).append(PATH_MESSAGE_BROKER_QUEUES) ),
   is_owner(true),
   queue_ids( std::string(directorio_de_trabajo).append(PATH_MESSAGE_BROKER).c_str(), 0, 0664, true),
   sem_set( std::vector<unsigned short>(1,1), std::string(directorio_de_trabajo).append(PATH_MESSAGE_BROKER).c_str(), 1, 0664) {
   
   create = create;

   sem_set.wait_on(0);
   for(int i=0; i<255; i++)
      memset(queue_ids->proj_id_to_queue_id[i], 0x00, 255);
   sem_set.signalize(0);
}

MessageBroker::MessageBroker(const char* directorio_de_trabajo) : 
   queues_path( std::string(directorio_de_trabajo).append(PATH_MESSAGE_BROKER_QUEUES) ),
   is_owner(false),
   queue_ids( std::string(directorio_de_trabajo).append(PATH_MESSAGE_BROKER).c_str(), 0, 0664),
   sem_set( std::string(directorio_de_trabajo).append(PATH_MESSAGE_BROKER).c_str(), 1, 1, 0664) { 

}

void  MessageBroker::create_queue(const char* queue_id) {

   try {
      char proj_id = allocate_queue_id(queue_id);
      // Log::info( "broker_create id='%s' proj_id='%c' (%d)", queue_id, proj_id, (int)proj_id);
      MessageQueue queue( queues_path.c_str(), (char)proj_id, 0664, true, false);
   } catch(std::exception error) {
      throw MessageBrokerError( "OSError: Error al crear la cola %s", error.what());
   }

}

void MessageBroker::destroy_queue(const char* queue_id) {
   try {
      char proj_id = get_proj_id(queue_id);
      // Log::info( "broker_destroy queue_id='%s' local_proj_id='%c' (%d)", queue_id, proj_id, (int)proj_id);
      MessageQueue queue( queues_path.c_str(), (char)proj_id, 0664, false, true);
      deallocate_queue_id(queue_id);

   } catch(std::exception error) {
      throw MessageBrokerError( "OSError: Error al destruir la cola %s", error.what());
   }
}

void MessageBroker::push(const char* queue_id, const void *msg, size_t size_txt) {
   sem_set.wait_on(0);
   if(!exist_queue(queue_id)) {
      Log::debug("No existe cola %s, para hacer push pero la creo", queue_id);
      create_queue(queue_id);
   }
   sem_set.signalize(0);

   try {
      char proj_id = get_proj_id(queue_id);
      // Log::info( "broker_push queue_id='%s' local_proj_id='%c' (%d)", queue_id, proj_id, (int)proj_id);
      MessageQueue queue( queues_path.c_str(), (char)proj_id, 0664, false, false);
      queue.push(msg, size_txt);
   } catch(std::exception error) {
      throw MessageBrokerError( "OSError: Error al hacer push() en la cola %s", error.what());
   }
}

ssize_t MessageBroker::pull(const char* queue_id, void *msg, size_t max_size_txt, long type) {
   ssize_t result = -1;

   sem_set.wait_on(0);
   if(!exist_queue(queue_id)) {
      Log::debug("No existe cola %s, para hacer pull pero la creo", queue_id);
      create_queue(queue_id);
   }
   sem_set.signalize(0);

   try {
      char proj_id = get_proj_id(queue_id);
      MessageQueue queue( queues_path.c_str(), (char)proj_id, 0664, false, false);

      // Log::info( "broker_pull queue_id='%s' local_proj_id='%c' (%d)", queue_id, proj_id, (int)proj_id);
      result = queue.pull(msg, max_size_txt, type);

   } catch(OSError error) {
      throw MessageBrokerError( "OSError: Error al hacer pull() en la cola %s", error.what());
   }

   return result;
}

   
char MessageBroker::allocate_queue_id(std::string queue_id) {
   int pos = 0;
   int proj_id = -1;

   sem_set.wait_on(0);
   
   while( (proj_id==-1) && (pos<255) ) {
      if( strlen(queue_ids->proj_id_to_queue_id[pos]) == 0 )
         proj_id = (char)pos;
      pos++;
   }
   
   if( proj_id == -1 ) {
      sem_set.signalize(0);
      throw MessageBrokerError("No hay colas disponibles  en el broker no puedo crear cola con id %s", queue_id.c_str());
   } else {
      strcpy( queue_ids->proj_id_to_queue_id[(int)proj_id], queue_id.c_str());
      sem_set.signalize(0);
   }

   return (char)proj_id;
}

void MessageBroker::deallocate_queue_id(std::string queue_id) {
   try {
      sem_set.wait_on(0);
      int pos = (int)get_proj_id(queue_id);
      memset(queue_ids->proj_id_to_queue_id[pos], 0x00, 255);
      sem_set.signalize(0);
   } catch(MessageBrokerError error) {
      sem_set.signalize(0);
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

bool MessageBroker::exist_queue(std::string queue_id) {
   int proj_id = -1;
   int pos = 0;
   
   while( (proj_id==-1) && (pos<255) ) {
      if( strcmp(queue_ids->proj_id_to_queue_id[pos], queue_id.c_str()) == 0 )
         proj_id = pos;
      pos++;
   }

   return (proj_id!=-1);
}


MessageBroker::~MessageBroker() {
  
   if(is_owner) {
      sem_set.wait_on(0);
      for(int i=0; i<255; i++) {
         if(queue_ids->proj_id_to_queue_id[i][0] != 0x00) {
            Log::info("Falto destruir cola queue_id=%s", queue_ids->proj_id_to_queue_id[i]);
            MessageQueue queue( queues_path.c_str(), (char)i, 0664, false, true);
            memset(queue_ids->proj_id_to_queue_id[i], 0x00, 255);
         }
      }
      sem_set.signalize(0);
   }
}
