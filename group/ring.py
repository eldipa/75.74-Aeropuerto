
import syslog

import struct
import time
import socket
from socket import AF_INET, SOCK_DGRAM, SOL_SOCKET, SO_BROADCAST, getaddrinfo
from invalid import *
import traceback
import get_port

LISTEN_TIMEOUT = 30 
LISTEN_QUEUE_LENGHT = 10


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

LISTEN_RETRIES = 10
assert LISTEN_RETRIES * LISTEN_TIMEOUT > SELF_CLOSE_TIMEWAIT 

CLOSE_TIMEOUT = 30
BEACON_BUF_MAX_SIZE = 1024 / 2
DOS_SLEEP = 0.0001

def create_beacon(beacon_type, group_id, leader_name_len, localhost_name_len, service_name_len, leader_name, localhost_name, service_name, with_local_name=True):
   assert len(beacon_type) == 4
   assert 0 < group_id < 2**16
   
   assert 0 < leader_name_len < 2**8
   assert len(leader_name) == leader_name_len

   if with_local_name:
      assert 0 < localhost_name_len < 2**8
      assert len(localhost_name) == localhost_name_len
      assert len(service_name) == service_name_len
   else:
      assert localhost_name_len == localhost_name == None
      assert service_name_len == service_name == None
   
   if with_local_name:
      return struct.pack('>4sHBBB%is%is%is' % (leader_name_len, localhost_name_len, service_name_len), beacon_type, group_id, leader_name_len, localhost_name_len, service_name_len, leader_name, localhost_name, service_name)
   else:
      return struct.pack('>4sHB%is' % (leader_name_len, ), beacon_type, group_id, leader_name_len, leader_name)


def tail(network_name, group_id, localhost_name, driver):
   leader_name = driver.leader_name
   
   datagram_socket = socket.socket(AF_INET, SOCK_DGRAM)
   listener = socket.socket()
   
   retries = 0
   try:
      datagram_socket.setsockopt(SOL_SOCKET, SO_BROADCAST, 1)

      previous_node = None
      listener.settimeout(LISTEN_TIMEOUT) 

      listener, service_name = get_port.bind(listener, localhost_name) 
      listener.listen(LISTEN_QUEUE_LENGHT)
      
      leader_name_len, localhost_name_len, service_name_len = len(leader_name), len(localhost_name), len(service_name)

      tail_beacon = create_beacon('OPEN', group_id, leader_name_len, localhost_name_len, service_name_len, leader_name, localhost_name, service_name)

      assert len(tail_beacon) == 4+2+1+1+1+leader_name_len+localhost_name_len+service_name_len
      assert len(tail_beacon) < BEACON_BUF_MAX_SIZE

      while not previous_node:
         time.sleep(DOS_SLEEP)
         syslog.syslog(syslog.LOG_INFO, "Sending OPEN beacon...")
         datagram_socket.sendto(tail_beacon, (network_name, get_port.BEACON_SERVICE))
         try:
            previous_node, _previous_node_address = listener.accept()
            syslog.syslog(syslog.LOG_INFO, "Connection accepted: %s connected to %s" % (_previous_node_address[0], localhost_name))
         except socket.timeout:
            previous_node = None
            retries += 1

            if retries >= LISTEN_RETRIES:
               raise UnstableChannel("It seems that nobody is listen the OPEN beacon.", None)
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
      datagram_socket, _ = get_port.bind(datagram_socket, "")

      start = time.time()
      while True:
         try:
            time.sleep(DOS_SLEEP)
            syslog.syslog(syslog.LOG_INFO, "Waiting for an OPEN beacon...")
            wrapped_msg, _ = datagram_socket.recvfrom(BEACON_BUF_MAX_SIZE) 
            peer_len, = struct.unpack('>H', wrapped_msg[:2])
            peer, = struct.unpack('>%is' % peer_len, wrapped_msg[2:peer_len+2])
            msg = wrapped_msg[peer_len+2:]
            try:
               syslog.syslog(syslog.LOG_DEBUG, "Packet received (%s): %s" % (str(peer), " ".join(map(lambda c: hex(ord(c)), msg))))
               type, external_group_id = struct.unpack('>4sH', msg[:6])
               
               if type != 'OPEN':
                  raise InvalidNetworkMessage("The message has a wrong type", msg, peer)

               if group_id != external_group_id:
                  syslog.syslog(syslog.LOG_DEBUG, "Packet OPEN received from other group, skipping. This group '%i', the other group '%i'" % (group_id, external_group_id))
                  #Otro grupo, skipping
                  continue

               remote_leader_name_len, remote_host_name_len, remote_service_name_len = struct.unpack('>BBB', msg[6:9])
               remote_leader_name, remote_host_name, remote_service_name = struct.unpack('>%is%is%is' % (remote_leader_name_len, remote_host_name_len, remote_service_name_len), msg[9:])
            except struct.error, e:
               #Mensaje invalido
               raise InvalidNetworkMessage("The message has a wrong format", msg, peer)

            time_elapsed = time.time() - start

            # Esto es valido solo para direcciones IP y por hostname
            is_myself = localhost_name == remote_host_name
            is_from_a_new_group = leader_name != remote_leader_name and not is_myself

            if (is_from_a_new_group or 
                  (not is_from_a_new_group and not is_myself and OWN_RING_CLOSE_TIMEWAIT < time_elapsed < SELF_CLOSE_TIMEWAIT) or
                  (is_myself and time_elapsed > SELF_CLOSE_TIMEWAIT)):

               k = '(new group)' if is_from_a_new_group else (
                     '(myself)' if is_myself and time_elapsed > SELF_CLOSE_TIMEWAIT else (
                     '(my group)'))
               
               next_node.settimeout(CLOSE_TIMEOUT)
               try:
                  try:
                     remote_service_name = socket.getservbyname(remote_service_name)
                  except socket.error:
                     remote_service_name = int(remote_service_name)

                  syslog.syslog(syslog.LOG_INFO, "Connecting to %s ..." % str(remote_host_name))
                  next_node.connect((remote_host_name, remote_service_name))
                  syslog.syslog(syslog.LOG_INFO, "Connection stablished with %s: %s (self) connected to %s [%s] (remote)." % (k, localhost_name, remote_host_name, remote_service_name))
                  return next_node
               except socket.error:
                  syslog.syslog(syslog.LOG_INFO, "Connection fail with %s [%s]. Retry." % (remote_host_name, remote_service_name))
                  #hubo un error de coneccion, ignorar y seguir buscando mas nodos
                  continue
               finally:
                  next_node.setblocking(1)
         
         except InvalidNetworkMessage, e:
            syslog.syslog(syslog.LOG_CRIT, "%s\n%s" % (traceback.format_exc(), str(e)))
   finally:
      datagram_socket.close()

