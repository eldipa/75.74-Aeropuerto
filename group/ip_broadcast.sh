#!/bin/bash

interfaz="$1"

if [ "$interfaz" == "" ] ; then
	
	echo "Falta la interfaz"
	exit -1
fi

num_linea_interfaz=`ifconfig | grep -n $interfaz | sed "s/^\([0-9]*\):.*/\1/"`
num_linea_direccion=`echo "$num_linea_interfaz + 1" | bc -l`

ip=`ifconfig | grep -n "inet:" | grep "^$num_linea_direccion:" | sed "s/.*Difus\.:\([^ ]*\).*/\1/"`

echo $ip

