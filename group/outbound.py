
import sys

sys.path.append("../ipc")

from ipc import MessageQueue
from subprocess import Popen

if __name__ == '__main__':
   sockets_seized = []

   path, char_id_out, group_id, local_address, protocol_name = sys.argv[1:]
   group_id = int(group_id)

   userland_outbound_queue = MessageQueue(path, char_id_out, 0644, False)

   leader_address = local_address
   
   while True:
      next_node = head(group_id, local_address, leader_address, protocol_name)
      passage_outbound_messages(next_node, userland_outbound_queue)
