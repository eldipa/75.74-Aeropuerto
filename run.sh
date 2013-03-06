#!/usr/bin/env sh

#./free.sh

if [ $# -gt 0 ];then
    CONFIG_FILE=`pwd`/$@
fi

cd processes

./main $CONFIG_FILE



