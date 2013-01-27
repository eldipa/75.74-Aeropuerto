
import sys

sys.path.append("../ipc")

from ipc import MessageQueue
from subprocess import Popen

import passage
import ring
import struct
import message

class Driver:
    def __init__(self, localhost_name):
        self.leader_name = self.localhost_name = localhost_name

    def handle_loop_message(self, loop_payload):
        '''This function will process the loop message and will determine if the message should be passed to the next stage (the next process)
           or it should be discarted. (Returning True or False).
           '''
        type = struct.unpack(">B", loop_payload[0])[0]

        if type == passage.LOOP_SUBTYPE_BY_NAME['Leader']:
            leader_name_len = struct.unpack('>B', loop_payload[1])[0]
            leader_name = struct.unpack('%is' % leader_name_len, loop_payload[2: leader_name_len+2])[0]

            if leader_name == self.localhost_name:
                #Stop the leader algorithm. 
                #
                # The inbound process MUST start sending its localname as leadername to the outbound queue.
                # When that message come back to the outbound, then the algorithm finish and localname is the leadername
                print "Leader:", leader_name
                self.leader_name = leader_name
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


    def create_leader_proposal_msj(self):
       s = struct.pack(">BB%is" % (len(localhost_name)), passage.LOOP_SUBTYPE_BY_NAME['Leader'], len(localhost_name), localhost_name)
       return message.pack(s, passage.ID_BY_TYPE['LOOP'])


if __name__ == '__main__':
   sockets_seized = []

   path, char_id_in, group_id, localhost_name, network_name= sys.argv[1:]
   group_id = int(group_id)

   # The 'char' id can be an integer or a letter.
   try:
      char_id_in = int(char_id_in)
   except ValueError:
      char_id_in = int(ord(char_id_in))

   # From the value of one char, we calculate the other
   assert 0 <= char_id_in < 128
   char_id_out = char_id_in + 128

   # Because the MessageQueue constructor expect a 'char', we do the translate
   char_id_in = chr(char_id_in)
   char_id_out = chr(char_id_out)

   userland_inbound_queue = MessageQueue(path, char_id_in, 0644, True)
   userland_outbound_queue = MessageQueue(path, char_id_out, 0644, True)
    
   driver = Driver(localhost_name)
   
   head_process = Popen(["python", "outbound.py", path, char_id_out, str(group_id), localhost_name])
   
   while True:
      previous_node = ring.tail(network_name, group_id, localhost_name, driver)
      userland_outbound_queue.push(driver.create_leader_proposal_msj())
      passage.passage_inbound_messages(previous_node, userland_inbound_queue, userland_outbound_queue, driver)

