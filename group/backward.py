
import sys

sys.path.append("../ipc")

from ipc import MessageQueue
from socket import socket, AF_INET, SOCK_DGRAM, timeout, SOL_SOCKET, SO_BROADCAST
from time import time
import os

from subprocess import check_output

# Usar el mismo path que en forward.py pero con otro char-id
path, char_id = "/home/martin/slax-English-US-7.0-i486.iso", "b"
group_name = "TheGroup"

interface = "wlan0"
lider_name = my_name = check_output(["ifconfig", interface]).split('\n')[1].split()[1].split(':')[1]
broadcast = ".".join(my_name.split('.')[:3] + ["255"]) # se asuma clase C sin subnets

dest_port = 8083

q = MessageQueue(path, char_id, 0644, True)

any_node = socket(AF_INET, SOCK_DGRAM)
any_node.setsockopt(SOL_SOCKET, SO_BROADCAST, 1)

while True:
   # 'Tail' stage, waiting to other node (linking the ring)
   previous_node = None
   listener = socket()
   listener.settimeout(5) #timeout, hay otra forma?
   listener.bind(('', dest_port)) # '' means any address
   listener.listen(10)

   while not previous_node:
      any_node.sendto('Tail Group: %s Leader: %s Sender: %s' % (group_name, lider_name, my_name), (broadcast, dest_port))
      try:
         #No hay ningun protocolo o verificacion, si se conecta, se asume que esta todo OK
         previous_node, _ = listener.accept() 
      except timeout:
         previous_node = None
         continue

   listener.shutdown(2)
   listener.close()
   
   #End of the 'Tail' stage,
   #Backwarding(?) the messages
   pid = os.fork();
   if pid == 0:
      while True: #forwarder subprocess
         msg = previous_node.recv(512)  #IMPORTANTE!!! el protocolo debe saber cuanto se debe recibir!!!
         print msg
         q.push(msg)
      os.exit(0)
   else:
      ack = True
      while ack: #keep alive subprocess
         ack = previous_node.sendall("ack") == None #algun formato para esto??
      
      print "SHUTDOWN"
      #close and break the link, and rebuild this part of the ring
      previous_node.shutdown(2)
      previous_node.close()
      os.system("kill -2 %i" % pid)

      #goto to the start
