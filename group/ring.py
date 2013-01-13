
import struct
import socket
from socket import AF_INET, SOCK_DGRAM, SOL_SOCKET, SO_BROADCAST

LISTEN_TIMEOUT = 30 
LISTEN_SERVICE = 8084
LISTEN_QUEUE_LENGHT = 10

BEACON_SERVICE = LISTEN_SERVICE

# Based (but not enforced) in the types of protocols used in an ARP request/reply. 
# See RFC 826
PROTOCOL_TYPES = {
      'IP' : 0x0800,
      'FQDN' : 0x0801,
      }

OWN_RING_CLOSE_TIMEWAIT = 1 * 60 # 1 minutes
SELF_CLOSE_TIMEWAIT = 2 * 60 # 2 minutes

assert OWN_RING_CLOSE_TIMEWAIT < SELF_CLOSE_TIMEWAIT

CLOSE_TIMEOUT = 30
BEACON_BUF_MAX_SIZE = 1024

def _create_beacon(beacon_type, group_id, protocol_type, leader_address_len, local_address_len, leader_address, local_address):
   assert len(beacon_type) == 4
   assert 0 < group_id < 2**16
   assert 0 < protocol_type < 2**16
   assert 0 < leader_address_len < 2**8
   assert 0 < local_address_len < 2**8

   assert len(leader_address) == leader_address_len
   assert len(local_address) == local_address_len

   return struct.pack('>4sHHBB%is%is' % (leader_address_len, local_address_len), beacon_type, group_id, protocol_type, leader_address_len, local_address_len, leader_address, local_address)


def tail(broadcast_address, group_id, protocol_name, local_address, leader_address):
   assert protocol_name in PROTOCOL_TYPES
   
   datagram_socket = socket.socket(AF_INET, SOCK_DGRAM)
   datagram_socket.setsockopt(SOL_SOCKET, SO_BROADCAST, 1)

   previous_node = None
   listener = socket.socket()
   listener.settimeout(LISTEN_TIMEOUT) 

   listener.bind((local_address, LISTEN_SERVICE)) 
   listener.listen(LISTEN_QUEUE_LENGHT)
   
   protocol_type = PROTOCOL_TYPES[protocol_name]
   leader_address_len, local_address_len = len(leader_address), len(local_address)

   tail_beacon = _create_beacon('TAIL', group_id, protocol_type, leader_address_len, leader_address, local_address_len, local_address)

   assert len(tail_beacon) == 4+2+2+1+1+leader_address_len+local_address_len
   assert len(tail_beacon) < BEACON_BUF_MAX_SIZE

   while not previous_node:
      datagram_socket.sendto(tail_beacon, (broadcast_address, BEACON_SERVICE))
      try:
         previous_node, _previous_node_address = listener.accept()
         print "Local Node: %s <-- External Node: %s" % (local_address, _previous_node_address)
      except timeout:
         previous_node = None
         continue

   listener.shutdown(2)
   listener.close()

   datagram_socket.close()

   return previous_node


def head(group_id, local_address, leader_address, protocol_name):
   assert protocol_name in PROTOCOL_TYPES

   datagram_socket = socket.socket(AF_INET, SOCK_DGRAM)
   datagram_socket.bind((local_address, LISTEN_SERVICE))

   next_node = socket()
   start = time()
   while True
      msg, _ = datagram_socket.recvfrom(BEACON_BUF_MAX_SIZE) 
      type, external_group_id, remote_protocol_type = struct.unpack('>4sHH', msg[:8])
      
      if type != 'TAIL' or group_id != external_group_id:
         #Mensaje invalido o de otro grupo

      if remote_protocol_type not in PROTOCOL_TYPES:
         # Tipo desconocido
      
      try:
         remote_leader_address_len, remote_host_address_len = struct.unpack('>BB', msg[8,10])
         remote_leader_address, remote_host_address = struct.unpack('>%is%is' % (remote_leader_address_len, remote_host_address_len), msg[10:])
      except struct.error:
         #Mensaje invalido

      time_elapsed = time() - start

      # Esto es valido solo para direcciones IP y por hostname
      # Para soportar otros tipos de protocolo, se debe codear lo necesario en funcion de 'remote_protocol_type y protocol_name'
      is_from_a_new_group = leader_address not in map(lambda t: t[3], getaddrinfo(remote_leader_address, LISTEN_SERVICE))
      is_myself = local_address in map(lambda t: t[3], getaddrinfo(remote_host_address, LISTEN_SERVICE)) 

      if (is_from_a_new_group or 
            (not is_from_a_new_group and not is_myself and OWN_RING_CLOSE_TIMEWAIT < time_elapsed < SELF_CLOSE_TIMEWAIT) or
            (is_myself and time_elapsed > SELF_CLOSE_TIMEWAIT)):
         
         next_node.settimeout(CLOSE_TIMEOUT)
         try:
            next_node.connect((remote_host_address, LISTEN_SERVICE))
            print "Local Node: %s --> External Node: %s" % (local_address, remote_host_address)
            return next_node
         except socket.error:
            #hubo un error de coneccion, ignorar y seguir buscando mas nodos
         finally:
            next_node.setblocking(1)

