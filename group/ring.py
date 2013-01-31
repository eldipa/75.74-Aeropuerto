
import struct
import time
import socket
from socket import AF_INET, SOCK_DGRAM, SOL_SOCKET, SO_BROADCAST, getaddrinfo
from invalid import *
import traceback

LISTEN_TIMEOUT = 30 
LISTEN_SERVICE = 8084
LISTEN_QUEUE_LENGHT = 10

BEACON_SERVICE = LISTEN_SERVICE

# Based (but not enforced) in the types of protocols used in an ARP request/reply. 
# See RFC 826
PROTOCOL_TYPES_BY_NAME = {
      'IP' : 0x0800,
      'FQDN' : 0x0801,
      }

PROTOCOL_TYPES = PROTOCOL_TYPES_BY_NAME.values()

OWN_RING_CLOSE_TIMEWAIT = 1 * 60 # 1 minutes
SELF_CLOSE_TIMEWAIT = 2 * 60 # 2 minutes

assert OWN_RING_CLOSE_TIMEWAIT < SELF_CLOSE_TIMEWAIT

CLOSE_TIMEOUT = 30
BEACON_BUF_MAX_SIZE = 1024 / 2
DOS_SLEEP = 0.0001

def _create_beacon(beacon_type, group_id, leader_name_len, localhost_name_len, leader_name, localhost_name):
   assert len(beacon_type) == 4
   assert 0 < group_id < 2**16
   assert 0 < leader_name_len < 2**8
   assert 0 < localhost_name_len < 2**8

   assert len(leader_name) == leader_name_len
   assert len(localhost_name) == localhost_name_len

   return struct.pack('>4sHBB%is%is' % (leader_name_len, localhost_name_len), beacon_type, group_id, leader_name_len, localhost_name_len, leader_name, localhost_name)


def tail(network_name, group_id, localhost_name, driver):
   leader_name = driver.leader_name
   
   datagram_socket = socket.socket(AF_INET, SOCK_DGRAM)
   listener = socket.socket()
   
   try:
      datagram_socket.setsockopt(SOL_SOCKET, SO_BROADCAST, 1)

      previous_node = None
      listener.settimeout(LISTEN_TIMEOUT) 

      listener.bind((localhost_name, LISTEN_SERVICE)) 
      listener.listen(LISTEN_QUEUE_LENGHT)
      
      leader_name_len, localhost_name_len = len(leader_name), len(localhost_name)

      tail_beacon = _create_beacon('OPEN', group_id, leader_name_len, localhost_name_len, leader_name, localhost_name)

      assert len(tail_beacon) == 4+2+1+1+leader_name_len+localhost_name_len
      assert len(tail_beacon) < BEACON_BUF_MAX_SIZE

      while not previous_node:
         time.sleep(DOS_SLEEP)
         datagram_socket.sendto(tail_beacon, (network_name, BEACON_SERVICE))
         try:
            previous_node, _previous_node_address = listener.accept()
            print "Local Node: %s <-- External Node: %s" % (localhost_name, _previous_node_address[0])
         except socket.timeout:
            previous_node = None
            continue
      
      return previous_node
   finally:
      listener.close()

      datagram_socket.close()



def head(group_id, localhost_name, driver):
   leader_name = driver.leader_name

   datagram_socket = socket.socket(AF_INET, SOCK_DGRAM)
   next_node = socket.socket()
   try:
      datagram_socket.bind(("", LISTEN_SERVICE))

      start = time.time()
      while True:
         try:
            time.sleep(DOS_SLEEP)
            msg, peer = datagram_socket.recvfrom(BEACON_BUF_MAX_SIZE) 
            try:
               type, external_group_id = struct.unpack('>4sH', msg[:6])
               
               if type != 'OPEN':
                  raise InvalidNetworkMessage("The message has a wrong type", msg, peer)

               if group_id != external_group_id:
                  #Otro grupo, skipping
                  continue

               remote_leader_name_len, remote_host_name_len = struct.unpack('>BB', msg[6:8])
               remote_leader_name, remote_host_name = struct.unpack('>%is%is' % (remote_leader_name_len, remote_host_name_len), msg[8:])
            except struct.error, e:
               #Mensaje invalido
               raise InvalidNetworkMessage("The message has a wrong format", msg, peer)

            time_elapsed = time.time() - start

            # Esto es valido solo para direcciones IP y por hostname
            is_from_a_new_group = leader_name != remote_leader_name
            is_myself = localhost_name == remote_host_name

            if (is_from_a_new_group or 
                  (not is_from_a_new_group and not is_myself and OWN_RING_CLOSE_TIMEWAIT < time_elapsed < SELF_CLOSE_TIMEWAIT) or
                  (is_myself and time_elapsed > SELF_CLOSE_TIMEWAIT)):

               k = '(new group)' if is_from_a_new_group else (
                     '(myself)' if is_myself and time_elapsed > SELF_CLOSE_TIMEWAIT else (
                     '(my group)'))
               
               next_node.settimeout(CLOSE_TIMEOUT)
               try:
                  next_node.connect((remote_host_name, LISTEN_SERVICE))
                  print "Local Node: %s --> External Node: %s %s" % (localhost_name, remote_host_name, k)
                  return next_node
               except socket.error:
                  #hubo un error de coneccion, ignorar y seguir buscando mas nodos
                  continue
               finally:
                  next_node.setblocking(1)
         
         except InvalidNetworkMessage, e:
            print e #TODO Log the exception, this is not critical.
            print traceback.format_exc()
   finally:
      datagram_socket.close()

