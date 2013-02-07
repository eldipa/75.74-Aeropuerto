import struct
import message

from socket import timeout
from invalid import *
import traceback
import socket

RECEIVE_TIMEOUT = 10*60*60 #10 minutes
SEND_TIMEOUT = 5*10*10 #5 minutes (should SEND_TIMEOUT < RECEIVE_TIMEOUT)
ISALIVE_TIMEOUT = 10 #10 seconds

MAX_PAYLOAD = 2**14

ID_BY_TYPE = {
      'LOOP' : 2,
      'USER' : 1,
      }

LOOP_SUBTYPE_BY_NAME = {
      'Leader' : 0,
      'LinkBroken' : 1,
      }

TYPE_BY_ID = dict(map(lambda type_id: (type_id[1], type_id[0]), ID_BY_TYPE.items()))

assert len(ID_BY_TYPE) == len(TYPE_BY_ID)

TTL = 255**2

def _recv(socket, length, peer):
   i = length
   chunks = []

   while i > 0:
      chunks.append(socket.recv(i))

      if not chunks[-1]:
         raise UnstableChannel("The channel is unstable or the other side has closed the connection in the middle of a transmittion. ", peer)
      
      i -= len(chunks[-1])

   return "".join(chunks)


def _send(socket, buf):
   socket.sendall(buf)


def passage_inbound_messages(inbound_socket, userland_inbound_queue, userland_outbound_queue, driver):
   try: 
      inbound_socket.settimeout(RECEIVE_TIMEOUT)
      peer = inbound_socket.getpeername()

      while True:
         try:
            start_receiving = False
            syslog.syslog(syslog.LOG_INFO, "Receiving packet...")
            type = _recv(inbound_socket, 4, peer)
            start_receiving = True

            if type not in ID_BY_TYPE.keys():
               raise InvalidNetworkMessage("The message received from a member of the ring has a wrong type (corrupted message).", type, peer) 

            size = struct.unpack('>H', _recv(inbound_socket, 2, peer))[0]

            if size > MAX_PAYLOAD:
               raise InvalidNetworkMessage("The message received from a member of the ring has a wrong value in the 'size' field (corrupted message).", size, peer) 
            
            payload =  _recv(inbound_socket, size, peer)

            if type == 'LOOP':
               ttl = struct.unpack('>H', payload[:2])[0]
               
               syslog.syslog(syslog.LOG_INFO, "LOOP packet recieved (TTL %i): %s" % (ttl, " ".join(map(lambda c: hex(ord(c))[2:], payload[2:]))))
               if ttl <= 0:
                  syslog.syslog(syslog.LOG_INFO, "LOOP packet (TTL %i) discarted.")
                  continue 
               
               if driver.handle_loop_message(payload[2:]):
                  syslog.syslog(syslog.LOG_INFO, "Forwarding LOOP packet.")
                  payload = struct.pack('>H', ttl - 1) + payload[2:]
                  userland_outbound_queue.push(message.pack(payload, ID_BY_TYPE[type]))
            elif type == 'USER':
               syslog.syslog(syslog.LOG_INFO, "USER packet recieved: %s" % (" ".join(map(lambda c: hex(ord(c))[2:], payload))))
               userland_inbound_queue.push(message.pack(payload, ID_BY_TYPE[type]))
            else:
               raise InvalidNetworkMessage("The message received from a member of the ring was corrupted.", payload, peer) 

         except timeout:
            if start_receiving:
               syslog.syslog(syslog.LOG_INFO, "Packet received partially because timeout")
               raise UnstableChannel("The other side (other peer) is not responding to the probes.", peer)
            else:
               syslog.syslog(syslog.LOG_INFO, "No packet received, checking if it is alive...")
               inbound_socket.settimeout(ISALIVE_TIMEOUT)
               try:
                  inbound_socket.send('?') # is alive the other side?
               except timeout:
                  raise UnstableChannel("The other side (other peer) is not responding to the probes.", peer)
               finally:
                  inbound_socket.settimeout(RECEIVE_TIMEOUT)
   finally:
      try:
         inbound_socket.shutdown(2)
      except socket.error:
         pass 
      inbound_socket.close()


def passage_outbound_messages(outbound_socket, userland_outbound_queue, driver):
   outbound_socket.settimeout(SEND_TIMEOUT)
   peer = outbound_socket.getpeername()

   try:
      while True:
         try:
            syslog.syslog(syslog.LOG_INFO, "Pulling packet...")
            id, payload = message.unpack(userland_outbound_queue.pull())

            if id not in TYPE_BY_ID:
               raise InvalidApplicationMessage("The type of the message in the queue is invalid.", id)

            if len(payload) > MAX_PAYLOAD:
               raise InvalidApplicationMessage("The lenght of the payload is greater than the expected.", payload)

            type = TYPE_BY_ID[id]
            if type == 'LOOP' and not driver.handle_loop_message(payload[2:]):
                continue #loop message dicarted

            size = struct.pack('>H', len(payload))
            
            assert len(type) == 4
            assert len(size) == 2

            syslog.syslog(syslog.LOG_INFO, "Sending %s packet: %s" (type, " ".join(map(lambda c: hex(ord(c))[2:], type+size+payload))))
            _send(outbound_socket, type+size+payload)

         except InvalidApplicationMessage, e:
            syslog.syslog(syslog.LOG_CRIT, "%s\n%s" % (traceback.format_exc(), str(e)))

   except timeout:
      raise UnstableChannel("The other side (other peer) is not responding with any ACK.", peer)
   except socket.error:
      raise UnstableChannel("The other side (other peer) is not responding. It seem to be down.", peer)
   finally:
      try:
         outbound_socket.shutdown(2)
      except socket.error:
         pass
      outbound_socket.close()
