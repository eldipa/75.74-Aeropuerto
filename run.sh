#!/usr/bin/env sh

if [ $# -gt 0 ];then
    CONFIG_FILE=`pwd`/$@
fi

cd processes

pkill message_broker_

./message_broker_launcher $CONFIG_FILE &
sleep 2

./main $CONFIG_FILE



