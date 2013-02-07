
import sys
import syslog

sys.path.append("../ipc")

from ipc import MessageQueue
from subprocess import Popen

import passage
import ring
import struct
import message
from invalid import *
import traceback
import time

import stop

ALREADY_ADDR_USED_ATTEMPTS = 10
ALREADY_ADDR_USED_SLEEP = 5
addr_attempts = 0

class Driver:
    def __init__(self, localhost_name, path, char_id_out, group_id):
        self.leader_name = self.localhost_name = localhost_name
        self.leader_process = None
        self.path, self.char_id_out, self.group_id = path, char_id_out, group_id

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

                self.clean()
                self.leader_process = Popen(["python", "leader.py", self.path, self.char_id_out, str(self.group_id), self.localhost_name])
                return False 


            if leader_name <= self.leader_name:
                return False
            else:
                self.clean()
                self.leader_name = leader_name
                return True
        elif type == passage.LOOP_SUBTYPE_BY_NAME['LinkBroken']:
            open_node_name_len = struct.unpack('>B', loop_payload[1])[0]
            open_node_name = struct.unpack('%is' % open_node_name_len, loop_payload[2: open_node_name_len+2])[0]

            if open_node_name == self.localhost_name:
               return False
            else:
               return True
        else:
            #Tipo incorrecto, como llego aqui?!?
            raise Exception

        return False


    def clean(self):
       stop.stop(self.leader_process)
       self.leader_process = None


    def create_leader_proposal_msj(self):
       s = struct.pack(">HBB%is" % (len(localhost_name)), passage.TTL, passage.LOOP_SUBTYPE_BY_NAME['Leader'], len(localhost_name), localhost_name)
       return message.pack(s, passage.ID_BY_TYPE['LOOP'])

    def create_linkbroken_msj(self):
       s = struct.pack(">HBB%is" % (len(localhost_name)), passage.TTL, passage.LOOP_SUBTYPE_BY_NAME['LinkBroken'], len(localhost_name), localhost_name)
       return message.pack(s, passage.ID_BY_TYPE['LOOP'])

if __name__ == '__main__':
   if len(sys.argv[1:]) != 5:
      print "Usage: inbound.py path char_id_in group_id localhost_name network_name"
      print "  - path: a full path to a file to be used as part of the key for the in/out queues."
      print "  - char_id_in: an integer or a character (converted in an int later) to be used as a part of the key of the inbound queue. The id used by the outbound queue will be that id+128."
      print "  - group_id: the id of the group"
      print "  - localhost_name: the name of this host viewed by other nodes."
      print "  - network_name: the name of the network, which message addressed to network_name will be delivery to any node in that network (broadcast address)."
      sys.exit(1)


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
   
   syslog.openlog("inbound")
   syslog.syslog(syslog.LOG_INFO, "Init 'inbound' process. Creating queues. Arguments: Path: %s Char_in_id: %s GroupId: %i Localhost: %s NetworkName: %s" % (
      path, hex(ord(char_id_in)), group_id, localhost_name, network_name))
   userland_inbound_queue = MessageQueue(path, char_id_in, 0644, True)
   userland_outbound_queue = MessageQueue(path, char_id_out, 0644, True)
    
   driver = Driver(localhost_name, path, char_id_out, group_id)
   
   head_process = Popen(["python", "outbound.py", path, char_id_out, str(group_id), localhost_name])
   
   try:
      while True:
         try:
            syslog.syslog(syslog.LOG_INFO, "Pushing 'BrokenLink' in the output queue.")
            userland_outbound_queue.push(driver.create_linkbroken_msj())

            syslog.syslog(syslog.LOG_INFO, "Construction the ring")
            previous_node = ring.tail(network_name, group_id, localhost_name, driver)
            syslog.syslog(syslog.LOG_INFO, "External node %s connected to me" % str(previous_node.getpeername()))

            addr_attempts = 0
            syslog.syslog(syslog.LOG_INFO, "Pushing 'LeaderElection' in the output queue.")
            userland_outbound_queue.push(driver.create_leader_proposal_msj())

            syslog.syslog(syslog.LOG_INFO, "Connection ready. Forwarding the messages.")
            passage.passage_inbound_messages(previous_node, userland_inbound_queue, userland_outbound_queue, driver)

         except InvalidMessage, e:
            syslog.syslog(syslog.LOG_CRIT, "%s\n%s" % (traceback.format_exc(), str(e)))
         except UnstableChannel, e:
            syslog.syslog(syslog.LOG_CRIT, "%s\n%s" % (traceback.format_exc(), str(e)))
         except KeyboardInterrupt:
            syslog.syslog(syslog.LOG_INFO, "Interruption:\n%s" % traceback.format_exc())
            sys.exit(0)
         except Exception, e:
            syslog.syslog(syslog.LOG_CRIT, "Critical exception (will shutdown everything) %s\n%s" % (traceback.format_exc(), str(e)))

            if hasattr(e, 'errno') and e.errno == 98:
               addr_attempts += 1
               syslog.syslog(syslog.LOG_INFO, "Address already used (attempts %i)" % addr_attempts)
               if addr_attempts < ALREADY_ADDR_USED_ATTEMPTS:
                  time.sleep(ALREADY_ADDR_USED_SLEEP)
                  continue
            
            sys.exit(2)

   finally:
      syslog.syslog(syslog.LOG_INFO, "Shutdown 'inbound'. Stoping other processes.")
      stop.stop(head_process)
      driver.clean()

