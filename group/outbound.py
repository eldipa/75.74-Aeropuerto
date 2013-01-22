
import sys

sys.path.append("../ipc")

from ipc import MessageQueue
from subprocess import Popen

import passage
import ring

class Driver:
    def __init__(self, leader_name):
        self.leader_name = leader_name


if __name__ == '__main__':
   sockets_seized = []

   path, char_id_out, group_id, localhost_name = sys.argv[1:]
   group_id = int(group_id)

   userland_outbound_queue = MessageQueue(path, char_id_out, 0644, False)

   driver = Driver(leader_name = localhost_name)
   
   while True:
      next_node = ring.head(group_id, localhost_name, driver)
      passage.passage_outbound_messages(next_node, userland_outbound_queue, driver)
