import sys

sys.path.append("../ipc")

from ipc import MessageQueue

import traceback
import stop
import ring
import socket
from socket import AF_INET, SOCK_DGRAM, SOL_SOCKET, SO_BROADCAST
import passage
import time
import inbound
import syslog
import struct
from invalid import *
import message
import get_port

LISTEN_TIMEOUT = 60
LEADER_REELECTION_MINIMUN = 60 * 5 #5 minutes

assert LISTEN_TIMEOUT < LEADER_REELECTION_MINIMUN


import signal
def handler(signum, frame):
   raise KeyboardInterrupt("Interrupted by a TERM signal.")

signal.signal(signal.SIGTERM, handler)


def create_breaklink_msj():
   s = struct.pack(">HB", 0, passage.LOOP_SUBTYPE_BY_NAME['BreakLinkForced'])
   return message.pack(s, passage.ID_BY_TYPE['LOOP'])

def socket_udp(broadcast):
   datagram_socket = socket.socket(AF_INET, SOCK_DGRAM)
   datagram_socket.settimeout(LISTEN_TIMEOUT) 

   if broadcast:
      datagram_socket.setsockopt(SOL_SOCKET, SO_BROADCAST, 1)
   else:
      datagram_socket, _ = get_port.bind(datagram_socket, "")

   return datagram_socket

if __name__ == '__main__':
   if len(sys.argv[1:]) != 5:
      print "Usage: leader.py path char_id_in group_id localhost_name "
      print "  - path: a full path to a file to be used as part of the key for the out queues."
      print "  - char_id_out: an integer or a character (converted in an int later) to be used as a part of the key of the outbound queue. "
      print "  - group_id: the id of the group (a no-negative)"
      print "  - localhost_name: the name of this host viewed by other nodes."
      print "  - network_name: the name of the network, which message addressed to network_name will be delivery to any node in that network (broadcast address)."
      print
      print "Note: you should NOT be executing this code by your self."
      sys.exit(1)

   
   path, char_id_out, group_id, localhost_name, network_name = sys.argv[1:]
   group_id = int(group_id)
   assert group_id >= 0

   syslog.openlog("leader")
   syslog.syslog(syslog.LOG_INFO, "Init 'leader' process. Creating queue. Arguments: Path: %s Char_out_id: %s GroupId: %i Localhost: %s NetworkName: %s" % (
      path, hex(ord(char_id_out)), group_id, localhost_name, network_name))

   userland_outbound_queue = MessageQueue(path, char_id_out, 0644, False)

   datagram_socket = None

   leader_beacon = ring.create_beacon('FIND', group_id, len(localhost_name), None, None, localhost_name, None, None, False)
   assert len(leader_beacon) < ring.BEACON_BUF_MAX_SIZE
 
   start_time = time.time()

   try:
      while True:
         try:
            syslog.syslog(syslog.LOG_INFO, "Sending FIND beacon...")
            datagram_socket = socket_udp(True)
            datagram_socket.sendto(leader_beacon, (network_name, get_port.BEACON_SERVICE))
            datagram_socket.close()
            
            syslog.syslog(syslog.LOG_INFO, "Waiting for a FIND beacon...")
            datagram_socket = socket_udp(False)
            wrapped_msg, _ = datagram_socket.recvfrom(ring.BEACON_BUF_MAX_SIZE) 
            peer_len, = struct.unpack('>H', wrapped_msg[:2])
            peer, = struct.unpack('>%is' % peer_len, wrapped_msg[2:peer_len+2])
            msg = wrapped_msg[peer_len+2:]
            datagram_socket.close()

            syslog.syslog(syslog.LOG_INFO, "Sending FIND beacon...")
            datagram_socket = socket_udp(True)
            datagram_socket.sendto(leader_beacon, (network_name, get_port.BEACON_SERVICE))
            datagram_socket.close()

            syslog.syslog(syslog.LOG_DEBUG, "Packet received (%s): %s" % (str(peer), " ".join(map(lambda c: hex(ord(c)), msg))))

            type, external_group_id = struct.unpack('>4sH', msg[:6])
            
            if type in ('OPEN', ):
               continue #drop

            if type != 'FIND':
               raise InvalidNetworkMessage("The message has a wrong type", msg, peer)

            if group_id != external_group_id:
               syslog.syslog(syslog.LOG_DEBUG, "Packet FIND received from other group, skipping. This group '%i', the other group '%i'" % (group_id, external_group_id))
               #Otro grupo, skipping
               continue

            remote_leader_name_len, = struct.unpack('>B', msg[6:7])
            remote_leader_name, = struct.unpack('>%is' % (remote_leader_name_len), msg[7:])
            
            if localhost_name == remote_leader_name:
               syslog.syslog(syslog.LOG_ERR, "Packet FIND received from myself!")
            
            
            syslog.syslog(syslog.LOG_INFO, "Pushing 'BreakLinkForced' in the output queue.")
            userland_outbound_queue.push(create_breaklink_msj())
            
            time.sleep(0.1)
            sys.exit(0)

         except socket.timeout:
            if time.time() - start_time >= LEADER_REELECTION_MINIMUN:
               syslog.syslog(syslog.LOG_INFO, "Pushing 'LeaderElection' (reelection) in the output queue.")
               userland_outbound_queue.push(inbound.create_leader_proposal_msj(localhost_name))
               start_time = time.time()

         except KeyboardInterrupt:
            syslog.syslog(syslog.LOG_INFO, "Interruption:\n%s" % traceback.format_exc())
            sys.exit(0)
      
         except Exception, e:
            syslog.syslog(syslog.LOG_CRIT, "Critical exception (will NOT shutdown) %s\n%s" % (traceback.format_exc(), str(e)))

            if hasattr(e, 'errno') and e.errno == 98:
               syslog.syslog(syslog.LOG_INFO, "Address already used")
               time.sleep(inbound.ALREADY_ADDR_USED_SLEEP)

   finally:
      try:
         datagram_socket.close()
      except:
         pass


