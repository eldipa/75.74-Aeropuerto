
import sys

sys.path.append("../ipc")

from ipc import MessageQueue
from socket import socket, AF_INET, SOCK_DGRAM, timeout, SOL_SOCKET, SO_BROADCAST
from time import time, sleep
import os, select

from subprocess import check_output

# Mucho hardcodeo?
path, char_id = "/home/martin/slax-English-US-7.0-i486.iso", "a"
group_name = "TheGroup"

#Esto es interesante, como saber 'mi nombre' o, en bajo nivel, 'mi ip'?
#Lo mismo para saber cual es la direccion del grupo multicast o bien la de broadcast
#Se puede asociar 'grupo'/'subnet' a una direccion de bajo nivel 'multicast'/'broadcast'?
interface = "wlan0"
lider_name = my_name = check_output(["ifconfig", interface]).split('\n')[1].split()[1].split(':')[1]
broadcast = ".".join(my_name.split('.')[:3] + ["255"]) # se asuma clase C sin subnets

#Puerto del protocolo??
dest_port = 8083

q = MessageQueue(path, char_id, 0644, True)

#interesante, setsockopt debe ser usado para que el socket pueda broadcastear
any_node = socket(AF_INET, SOCK_DGRAM)
any_node.setsockopt(SOL_SOCKET, SO_BROADCAST, 1)
any_node.bind(('', dest_port))

while True:
   # 'Head' stage, connecting to other node (linking the ring)
   next_node = socket()
   connected = False
   start = time()
   wait_time = 30
   while not connected:
      #definir un mensaje estadar (facil de parsear)
      #
      #reescribir este ciclo, puede quedar ciclando, emitiendo broadcast a lo pavote.
      sleep(2) # hay que evitar el floodeo de broadcast
      any_node.sendto('Head Group: %s Leader: %s Sender: %s' % (group_name, lider_name, my_name), (broadcast, dest_port))
      any_node.settimeout(20) #timeout, hay otra forma?
      try:
         msg, who = any_node.recvfrom(60) #512?? cuanto deberia recibir?
      except timeout:
         continue
      finally:
         any_node.setblocking(1)
      
      try:
         _kind, _, _group, _, _leader, _, _sender = msg.split()
      except:
         #que se hace si se reciben mensajes incorrectos?
         print "[Abnormal message recieved]", msg
         continue

      #rechequear esto:
      if _kind == 'Tail' and _group == group_name:
         if (_leader != lider_name and _leader != my_name) or ((_leader == lider_name or _leader == my_name) and time() - start > wait_time):
            next_node.connect((_sender, dest_port))
            connected = True
   
   #End of the 'Head' stage,
   #Forwarding the messages
   pid = os.fork();
   if pid == 0:
      while True: #forwarder subprocess
         msg = q.pull()
         next_node.sendall(msg)
      os.exit(0)
   else:
      read_ready = True
      while read_ready: #keep alive subprocess
         read_ready, _, _ = select.select([next_node.fileno()], [], [], 60)
         if read_ready: #keep alive recieved
            next_node.recv(512)
 
      print "SHUTDOWN"
      #timeout, close and break the link, and rebuild this part of the ring
      next_node.shutdown(2)
      next_node.close()
      os.system("kill -2 %i" % pid)

      #goto to the start

