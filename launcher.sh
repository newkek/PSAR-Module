#!/bin/bash

MOD=ibs_measure_module
# MOD=libaddress

clear

if [ "$1" == "-a" ] || [ "$1" == "add" ]; then
  make
  if [ $? -eq 0 ]; then
    sudo insmod $MOD.ko
  else
    exit 1
  fi
elif [ "$1" == "-d" ] || [ "$1" == "del" ]; then
  sudo rmmod $MOD
elif [ "$1" == "-c" ] || [ "$1" == "clean" ]; then
  make clean
elif [ "$1" == "-h" ] || [ "$1" == "help" ]; then
  echo "$0   [-a|add]   : add kernel module
                [-d|del]   : delete kernel module
                [-c|clean] : make clean
                [-h|help]  : print this message"
else
  make
fi

exit 0