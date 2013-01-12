
RECEIVE_TIMEOUT = 10*60*60 #10 minutes
ISALIVE_TIMEOUT = 10 #10 seconds

def _recv(socket, length):
   i = length
   chunks = []

   while i > 0:
      chunks.append(socket.recv(i))

      if not chunks[-1]:
         #ERROR. Coneccion cerrada?
      
      i -= len(chunks[-1])

   return "".join(chunks)


def passage_inbound_messages(inbound_socket, userland_inbound_queue, userland_outbound_queue):
   inbound_socket.settimeout(RECEIVE_TIMEOUT)
   while True:
      try:
         type = _recv(inbound_socket, 4)

         if type not in ('MOM1', 'USER'):
            #ERROR. Mensaje corrupto o desincronizado?

         size = struct.unpack('>I', _recv(inbound_socket, 4))

         if size > 2**14:
            #ERROR. Mensaje corrupto o desincronizado? El size supera el limite definido (basado en el limite del payload de los mensajes que se pueden pushear en una cola compartida.

         payload =  _recv(inbound_socket, size)

         if type == 'MOM1':
            userland_outbound_queue.push(payload)
         elif type == 'USER':
            userland_inbound_queue.push(payload)
         else:
            assert False

      except timeout:
         inbound_socket.settimeout(ISALIVE_TIMEOUT)
         try:
            inbound_socket.send('?') # is alive the other side?
         except timeout:
            #ERROR. El anillo esta ROTO
         finally:
            inbound_socket.settimeout(RECEIVE_TIMEOUT)


def passage_outbound_messages(outbound_socket, userland_outbound_queue):
   payload = 
