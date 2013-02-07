
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

   syslog.openlog("outbound")
   syslog.syslog(syslog.LOG_INFO, "Init 'outbound' process. Creating queue. Arguments: Path: %s Char_out_id: %s GroupId: %i Localhost: %s" % (
      path, char_id_out, group_id, localhost_name))
   userland_outbound_queue = MessageQueue(path, char_id_out, 0644, False)

   driver = Driver(localhost_name)
   while True:
      try:
         syslog.syslog(syslog.LOG_INFO, "Construction the ring")
         next_node = ring.head(group_id, localhost_name, driver)

         syslog.syslog(syslog.LOG_INFO, "Connection ready. Forwarding the messages.")
         passage.passage_outbound_messages(next_node, userland_outbound_queue, driver)

      except InvalidMessage, e:
         syslog.syslog(syslog.LOG_CRIT, "%s\n%s" % (traceback.format_exc(), str(e)))
      except UnstableChannel, e:
         syslog.syslog(syslog.LOG_CRIT, "%s\n%s" % (traceback.format_exc(), str(e)))
      except KeyboardInterrupt:
         syslog.syslog(syslog.LOG_INFO, "Interruption:\n%s" % traceback.format_exc())
         sys.exit(0)
      except Exception, e:
         syslog.syslog(syslog.LOG_CRIT, "Critical exception (will NOT shutdown) %s\n%s" % (traceback.format_exc(), str(e)))

