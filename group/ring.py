
import struct
import socket

LISTEN_TIMEOUT = 30 
LISTEN_SERVICE = 8084
LISTEN_QUEUE_LENGHT = 10

BEACON_SERVICE = LISTEN_SERVICE

PROTOCOL_TYPES = [
      'IP' : (0x0800, 4)
      ]



def _common_beacon(beacon_type, group_id, protocol_type, address_len, leader_address, local_address):
   assert len(beacon_type) == 4
   assert 0 < group_id < 2**16
   assert 0 < protocol_type < 2**16
   assert 0 < address_len < 2**8

   assert len(leader_address) == len(local_address) == address_len

   return struct.pack('>4sHHB%is%is' % (address_len, address_len), beacon_type, group_id, protocol_type, address_len, leader_address, local_address)


def tail(datagram_socket, broadcast_address, group_id, protocol_name, local_address, leader_address):
   assert protocol_name in PROTOCOL_TYPES

   previous_node = None
   listener = socket.socket()
   listener.settimeout(LISTEN_TIMEOUT) 

   listener.bind((local_address, LISTEN_SERVICE)) 
   listener.listen(LISTEN_QUEUE_LENGHT)
   
   protocol_type, address_len = PROTOCOL_TYPES[protocol_name]
   tail_beacon = _common_beacon('TAIL', group_id, protocol_type, address_len, leader_address, local_address)

   assert len(tail_beacon) == 4+2+2+1+2*address_len

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

   return previous_node
