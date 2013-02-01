
import sys

sys.path.append("../ipc")

from ipc import MessageQueue
from subprocess import Popen

import passage
import ring
import struct
from invalid import *
import traceback

class Driver:
    def __init__(self, localhost_name):
        self.leader_name = self.localhost_name = localhost_name 

    def handle_loop_message(self, loop_payload):
        type = struct.unpack(">B", loop_payload[0])[0]

        if type == passage.LOOP_SUBTYPE_BY_NAME['Leader']:
            leader_name_len = struct.unpack('>B', loop_payload[1])[0]
            leader_name = struct.unpack('%is' % leader_name_len, loop_payload[2: leader_name_len+2])[0]
            
            if leader_name == self.localhost_name:
                # Forwarding the message
                return True


            if leader_name <= self.leader_name:
                return False
            else:
                self.leader_name = leader_name
                return True
        elif type == passage.LOOP_SUBTYPE_BY_NAME['LinkBroken']:
            return True
        else:
            #Tipo incorrecto, como llego aqui?!?
            raise Exception

        return False

if __name__ == '__main__':
   if len(sys.argv[1:]) != 4:
      print "Usage: outbound.py path char_id_in group_id localhost_name "
      print "  - path: a full path to a file to be used as part of the key for the out queues."
      print "  - char_id_out: an integer or a character (converted in an int later) to be used as a part of the key of the outbound queue. "
      print "  - group_id: the id of the group"
      print "  - localhost_name: the name of this host viewed by other nodes."
      print
      print "Note: you should NOT be executing this code by your self."
      sys.exit(1)

   path, char_id_out, group_id, localhost_name = sys.argv[1:]
   group_id = int(group_id)

   userland_outbound_queue = MessageQueue(path, char_id_out, 0644, False)

   driver = Driver(localhost_name)
   while True:
      try:
         next_node = ring.head(group_id, localhost_name, driver)
         passage.passage_outbound_messages(next_node, userland_outbound_queue, driver)
      except InvalidMessage, e:
         print e #TODO Log the exception, this is not critical.
         print traceback.format_exc()
      except UnstableChannel, e:
         print e #TODO Log the exception, this is not critical.
         print traceback.format_exc()
      except KeyboardInterrupt:
         print traceback.format_exc()
         sys.exit(0)
      except Exception, e:
         print e #TODO Critical?
         print traceback.format_exc()


