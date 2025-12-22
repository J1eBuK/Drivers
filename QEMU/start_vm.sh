#!/bin/bash

echo "Starting Alpine VM with 2 serial-ports..."
echo "Serial0: stdio (cmd)"
echo "Serial1: TCP socket on port 4444/4445"

qemu-system-x86_64 \
    -machine pc \
    -cpu max \
    -m 512 \
    -drive file=ubuntu-test.qcow2,format=qcow2,if=virtio \
    -device isa-serial,chardev=char0 \
    -chardev stdio,mux=on,id=char0,signal=off \
    -device isa-serial,chardev=char1 \
    -chardev socket,id=char1,port=4444,host=0.0.0.0,server=on,wait=off \
    -device isa-serial,chardev=char2 \
    -chardev socket,id=char2,port=4445,host=0.0.0.0,server=on,wait=off \
    -display none

echo "VM killed"