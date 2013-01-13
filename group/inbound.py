
import sys

sys.path.append("../ipc")

from ipc import MessageQueue
from subprocess import Popen

if __name__ == '__main__':
   sockets_seized = []

   path, char_id_in, char_id_out, group_id, local_address, broadcast_address, protocol_name = sys.argv[1:]
   group_id = int(group_id)

   userland_inbound_queue = MessageQueue(path, char_id_in, 0644, True)
   userland_outbound_queue = MessageQueue(path, char_id_out, 0644, True)

   head = Popen([]) 
   leader_address = local_address
   
   while True:
      previous_node = tail(broadcast_address, group_id, protocol_name, local_address, leader_address)
      passage_inbound_messages(previous_node, userland_inbound_queue, userland_outbound_queue)

