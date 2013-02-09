import struct
import message
import syslog

from socket import timeout
from invalid import *
import traceback
import socket

KEEPALIVE_PROBE_COUNT = 8
KEEPALIVE_IDLE_TIMEOUT = 1 * 60 # Value with a range of 1 to N (where N is tcp_keepidle divided by PR_SLOWHZ).
KEEPALIVE_RETRY_SEND_PROBE = 10 # Value with a range of 1 to N (where N is tcp_keepintvl divided by PR_SLOWHZ).

MAX_PAYLOAD = 2**14

ID_BY_TYPE = {
      'LOOP' : 2,
      'USER' : 1,
      }

LOOP_SUBTYPE_BY_NAME = {
      'Leader' : 0,
      'LinkBroken' : 1,
      'BreakLinkForced' : 2,
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


def _send(socket, buf, peer):
   i = 0
   s = 0
   while i < len(buf):
      s = socket.send(buf[i:])
      if s == 0:
         raise UnstableChannel("The channel is unstable or the other side has closed the connection in the middle of a transmittion. ", peer)
      if s < 0:
         raise UnstableChannel("The channel is unstable because an unknow error (may be local, may be remote, or may be in the link). ", peer)
      
      i += s


def passage_inbound_messages(inbound_socket, userland_inbound_queue, userland_outbound_queue, driver):
   try: 
      inbound_socket.setsockopt(socket.IPPROTO_TCP, socket.TCP_NODELAY, 1)

      inbound_socket.setsockopt(socket.SOL_SOCKET, socket.SO_KEEPALIVE, 1)
      inbound_socket.setsockopt(socket.SOL_TCP, socket.TCP_KEEPCNT, KEEPALIVE_PROBE_COUNT)
      inbound_socket.setsockopt(socket.SOL_TCP, socket.TCP_KEEPIDLE, KEEPALIVE_IDLE_TIMEOUT)
      inbound_socket.setsockopt(socket.SOL_TCP, socket.TCP_KEEPINTVL, KEEPALIVE_RETRY_SEND_PROBE)

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
            _send(inbound_socket, "A", peer)

            if type == 'LOOP':
               ttl = struct.unpack('>H', payload[:2])[0]
               
               syslog.syslog(syslog.LOG_INFO, "LOOP packet recieved (TTL %i): %s" % (ttl, " ".join(map(lambda c: hex(ord(c)), payload[2:]))))
               if ttl <= 0:
                  syslog.syslog(syslog.LOG_INFO, "LOOP packet (TTL %i) discarted." % ttl)
                  continue 
               
               if driver.handle_loop_message(payload[2:]):
                  syslog.syslog(syslog.LOG_INFO, "Forwarding LOOP packet.")
                  payload = struct.pack('>H', ttl - 1) + payload[2:]
                  userland_outbound_queue.push(message.pack(payload, ID_BY_TYPE[type]))
            elif type == 'USER':
               syslog.syslog(syslog.LOG_INFO, "USER packet recieved: %s" % (" ".join(map(lambda c: hex(ord(c)), payload))))
               userland_inbound_queue.push(message.pack(payload, ID_BY_TYPE[type]))
            else:
               raise InvalidNetworkMessage("The message received from a member of the ring was corrupted.", payload, peer) 

         except socket.error, e:
            if start_receiving:
               syslog.syslog(syslog.LOG_INFO, "Packet received partially because some connection error.")
            else:
               syslog.syslog(syslog.LOG_INFO, "No packet received because some connection error.")

            raise UnstableChannel("The other side (other peer) is not responding to the probes or an internal error happen.", peer, e)

   finally:
      try:
         inbound_socket.shutdown(2)
      except socket.error:
         pass 
      inbound_socket.close()


def passage_outbound_messages(outbound_socket, userland_outbound_queue, driver):
   try:
      outbound_socket.setsockopt(socket.IPPROTO_TCP, socket.TCP_NODELAY, 1)
      peer = outbound_socket.getpeername()

      outbound_socket.setsockopt(socket.SOL_SOCKET, socket.SO_KEEPALIVE, 1)
      outbound_socket.setsockopt(socket.SOL_TCP, socket.TCP_KEEPCNT, KEEPALIVE_PROBE_COUNT)
      outbound_socket.setsockopt(socket.SOL_TCP, socket.TCP_KEEPIDLE, KEEPALIVE_IDLE_TIMEOUT)
      outbound_socket.setsockopt(socket.SOL_TCP, socket.TCP_KEEPINTVL, KEEPALIVE_RETRY_SEND_PROBE)

      while True:
         try:
            syslog.syslog(syslog.LOG_INFO, "Pulling packet...")
            id, payload = message.unpack(userland_outbound_queue.pull())

            if id not in TYPE_BY_ID:
               raise InvalidApplicationMessage("The type of the message in the queue is invalid.", id)

            if len(payload) > MAX_PAYLOAD:
               raise InvalidApplicationMessage("The lenght of the payload is greater than the expected.", payload)

            type = TYPE_BY_ID[id]
            if type == 'LOOP' and struct.unpack(">B", payload[2])[0] == LOOP_SUBTYPE_BY_NAME['BreakLinkForced']:
               syslog.syslog(syslog.LOG_INFO, "Breaking the link forced. Shutdown the socket...")
               return

            if type == 'LOOP' and not driver.handle_loop_message(payload[2:]):
                continue #loop message dicarted

            size = struct.pack('>H', len(payload))
            
            assert len(type) == 4
            assert len(size) == 2

            syslog.syslog(syslog.LOG_INFO, "Sending %s packet: %s" % (type, " ".join(map(lambda c: hex(ord(c)), type+size+payload))))
            _send(outbound_socket, type+size+payload, peer)
            ack = _recv(outbound_socket, 1, peer)
            if ack != "A":
               raise InvalidNetworkMessage("The ACK has is invalid '%s' for the message sent" % hex(ord(ack)), type+size+payload, peer)

         except InvalidApplicationMessage, e:
            syslog.syslog(syslog.LOG_CRIT, "%s\n%s" % (traceback.format_exc(), str(e)))

   except socket.error, e:
      raise UnstableChannel("The other side (other peer) is not responding. It seem to be down.", peer, e)
   finally:
      try:
         outbound_socket.shutdown(2)
      except socket.error:
         pass
      outbound_socket.close()
