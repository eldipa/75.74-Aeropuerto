import sys

sys.path.append("../ipc")

from ipc import MessageQueue

import traceback
import stop



if __name__ == '__main__':
   if len(sys.argv[1:]) != 4:
      print "Usage: leader.py path char_id_in group_id localhost_name "
      print "  - path: a full path to a file to be used as part of the key for the out queues."
      print "  - char_id_out: an integer or a character (converted in an int later) to be used as a part of the key of the outbound queue. "
      print "  - group_id: the id of the group"
      print "  - localhost_name: the name of this host viewed by other nodes."
      print
      print "Note: you should NOT be executing this code by your self."
      sys.exit(1)

   
   path, char_id_out, group_id, localhost_name = sys.argv[1:]
   print "Running Leader Process:", localhost_name

   userland_outbound_queue = MessageQueue(path, char_id_out, 0644, False)
   
   # enviar un mensaje diciendo (cada cierto tiempo deberia pedir reelecciones)
   # enviar un mensaje diciendo un beacon con su hostname y el group_id (ver puerto abajo)
   #
   # escucha mensajes en un puerto DISTINTO al puerto UDP usado por inbound/outbound
