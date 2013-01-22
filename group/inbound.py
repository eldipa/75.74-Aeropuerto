
import sys

sys.path.append("../ipc")

from ipc import MessageQueue
from subprocess import Popen

import passage
import ring

if __name__ == '__main__':
   sockets_seized = []

   path, char_id_in, char_id_out, group_id, localhost_name, network_name= sys.argv[1:]
   group_id = int(group_id)

   userland_inbound_queue = MessageQueue(path, char_id_in, 0644, True)
   userland_outbound_queue = MessageQueue(path, char_id_out, 0644, True)
    
   leader_name = localhost_name
   
   head_process = Popen(["python", "outbound.py", path, char_id_out, str(group_id), localhost_name])
   
   while True:
      previous_node = ring.tail(network_name, group_id, localhost_name, leader_name)
      passage.passage_inbound_messages(previous_node, userland_inbound_queue, userland_outbound_queue)

