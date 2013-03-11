#!/usr/bin/env sh

#./free.sh

if [ $# -gt 0 ];then
    CONFIG_FILE=`pwd`/$@
else
    CONFIG_FILE=`pwd`/"start_process.txt"
fi

cd processes

./run.py $CONFIG_FILE

#./main $CONFIG_FILE



