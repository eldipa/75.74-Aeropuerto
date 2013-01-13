import struct
import message

from socket import timeout

RECEIVE_TIMEOUT = 10*60*60 #10 minutes
SEND_TIMEOUT = 5*10*10 #5 minutes (should SEND_TIMEOUT < RECEIVE_TIMEOUT)
ISALIVE_TIMEOUT = 10 #10 seconds

MAX_PAYLOAD = 2**14

ID_BY_TYPE = {
      'MOM1' : 2,
      'USER' : 1,
      }

TYPE_BY_ID = dict(map(lambda type_id: (type_id[1], type_id[0]), ID_BY_TYPE.items()))

assert len(ID_BY_TYPE) == len(TYPE_BY_ID)

def _recv(socket, length):
   i = length
   chunks = []

   while i > 0:
      chunks.append(socket.recv(i))

      if not chunks[-1]:
         #ERROR. Coneccion cerrada?
         #XXX TODO esto es un bug, en algunas ocaciones recv retorna vacio (varias veces) entrando _recv en ciclo infinito
         raise Exception
      
      i -= len(chunks[-1])

   return "".join(chunks)


def _send(socket, buf):
   socket.sendall(buf)


def passage_inbound_messages(inbound_socket, userland_inbound_queue, userland_outbound_queue):
   inbound_socket.settimeout(RECEIVE_TIMEOUT)
   try: 
      while True:
         try:
            type = _recv(inbound_socket, 4)

            if type not in ID_BY_TYPE.keys():
               #ERROR. Mensaje corrupto o desincronizado?
               raise Exception

            size = struct.unpack('>H', _recv(inbound_socket, 2))

            if size > MAX_PAYLOAD:
               #ERROR. Mensaje corrupto o desincronizado? El size supera el limite definido (basado en el limite del payload de los mensajes que se pueden pushear en una cola compartida.
               raise Exception

            payload =  _recv(inbound_socket, size)

            if type == 'MOM1':
               userland_outbound_queue.push(message.pack(payload, ID_BY_TYPE[type]))
            elif type == 'USER':
               userland_inbound_queue.push(message.pack(payload, ID_BY_TYPE[type]))
            else:
               assert False

         except timeout:
            inbound_socket.settimeout(ISALIVE_TIMEOUT)
            try:
               inbound_socket.send('?') # is alive the other side?
            except timeout:
               #ERROR. El anillo esta ROTO
               raise Exception
            finally:
               inbound_socket.settimeout(RECEIVE_TIMEOUT)
   finally:
      inbound_socket.shutdown(2)
      inbound_socket.close()


def passage_outbound_messages(outbound_socket, userland_outbound_queue):
   outbound_socket.settimeout(SEND_TIMEOUT)

   while True:
      try:
         id, payload = message.unpack(userland_outbound_queue.pull())

         if id not in TYPE_BY_ID:
            #ERROR alguien, de alguna manera, metio un mensjae en la cola con un tipo inesperado
            raise Exception

         if len(payload) > MAX_PAYLOAD:
            #ERROR alguien, de alguna manera, metio un mensjae en la cola demasiado grande
            raise Exception

         type = TYPE_BY_ID[id]
         size = struct.pack('>H', len(payload))
         
         assert len(type) == 4
         assert len(size) == 2

         _send(outbound_socket, type)
         _send(outbound_socket, size)
         _send(outbound_socket, payload)
      
      except timeout:
         #ERROR, el anillo esta ROTO
         raise Exception
