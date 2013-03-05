#!/bin/bash

INTERFAZ="wlan0"
IP_LOCAL=`./ip_local.sh $INTERFAZ`
IP_BROADCAST=`./ip_broadcast.sh $INTERFAZ`

echo "python inbound.py ../processes/locks/cinta_central.lck 0 1 $IP_LOCAL $IP_BROADCAST"
python inbound.py ../local_broker/locks/colas_broker.lck 0 1 $IP_LOCAL $IP_BROADCAST
