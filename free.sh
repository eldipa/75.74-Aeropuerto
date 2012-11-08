#!/bin/bash

echo "Liberando recursos..."

echo "Liberando memoria compartida"
for i in `ipcs -m | grep "0x.*" | sed "s/\(0x[0-9a-f]*\).*/\1/" | grep  -v ^0x0*$`; 
do
    ipcrm -M ${i}
done

echo "Liberando semaforos"
for i in `ipcs -s | grep "0x.*" | sed "s/\(0x[0-9a-f]*\).*/\1/" | grep  -v ^0x0*$`; 
do
    ipcrm -S ${i}
done

echo "Liberando colas de mensajes"
for i in `ipcs -q | grep "0x.*" | sed "s/\(0x[0-9a-f]*\).*/\1/" | grep  -v ^0x0*$`; 
do
    ipcrm -Q ${i}
done

