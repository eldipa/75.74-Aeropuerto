
import sys

sys.path.append("../ipc")

from ipc import MessageQueue
from subprocess import Popen

import passage
import ring

class Driver:
    def __init__(self, localhost_name):
        self.leader_name = self.localhost_name = localhost_name

    def handle_loop_message(self, loop_payload):
        '''This function will process the loop message and will determine if the message should be passed to the next stage (the next process)
           or it should be discarted. (Returning True or False).
           '''
        type = struct.unpack(">B", loop_payload[0])

        if type == TYPE_BY_NAME['Leader']:
            leader_name_len = struct.unpack('>B', loop_payload[1])
            leader_name = struct.unpack('%is' % leader_name_len, loop_payload[2: leader_name_len+2])

            if leader_name == self.localhost_name:
                #Stop the leader algorithm. 
                #
                # The inbound process MUST start sending its localname as leadername to the outbound queue.
                # When that message come back to the outbound, then the algorithm finish and localname is the leadername
                return False 


            if leader_name <= self.leader_name:
                return False
            else:
                self.leader_name = leader_name
                return True
        else:
            #Tipo incorrecto, como llego aqui?!?
            raise Exception

        return False


if __name__ == '__main__':
   sockets_seized = []

   path, char_id_in, char_id_out, group_id, localhost_name, network_name= sys.argv[1:]
   group_id = int(group_id)

   userland_inbound_queue = MessageQueue(path, char_id_in, 0644, True)
   userland_outbound_queue = MessageQueue(path, char_id_out, 0644, True)
    
   driver = Driver(localhost_name)
   
   head_process = Popen(["python", "outbound.py", path, char_id_out, str(group_id), localhost_name])
   
   while True:
      previous_node = ring.tail(network_name, group_id, localhost_name, driver)
      passage.passage_inbound_messages(previous_node, userland_inbound_queue, userland_outbound_queue, driver)

