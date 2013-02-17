

#include "valueerror.h"
#include <cstring>
#include "group_interface.h"

#include <arpa/inet.h>
#include <netinet/in.h> // See man byteorder(3)

namespace {
   typedef struct {
      long type;
      char msg[GROUP_MAX_MESSAGE_SIZE];
   } MSG;
}

GroupInterface::GroupInterface(const char *absolute_path, char proj_id) :
   inbound(absolute_path, proj_id),
   outbound(absolute_path, char(proj_id + 128)) {

      if(proj_id < 0)
         throw ValueError("The project id is out of range. It must be in between 0 and 127 (inclusive) but the id is %i (path: %s)", proj_id, absolute_path);
   }

void GroupInterface::push(const char *msg, size_t size_msg) {
   if(size_msg > GROUP_MAX_MESSAGE_SIZE)
      throw ValueError("The message is too big to be sent. Its size is %i bytes.", size_msg);

   MSG qpack;
   memset(&qpack, 0, sizeof(MSG));

   qpack.type = htonl(1);
   memcpy(&qpack.msg, msg, size_msg);

   outbound.push(&qpack, size_msg+sizeof(long));
}

ssize_t GroupInterface::pull(char *msg, size_t max_size_msg) {
   if(max_size_msg > GROUP_MAX_MESSAGE_SIZE)
      throw ValueError("The expected size of the message is too big. Its size is %i bytes but the implementation can only recieve message of (at most) %i bytes.", max_size_msg, GROUP_MAX_MESSAGE_SIZE);

   MSG qpack;
   memset(&qpack, 0, sizeof(MSG));

   ssize_t recv = inbound.pull(&qpack, max_size_msg+sizeof(long), htonl(1));

   memcpy(msg, &qpack.msg, recv);
   return recv;
}
