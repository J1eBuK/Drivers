#!/bin/bash

echo "Starting Alpine VM with 2 serial-ports..."
echo "Serial0: stdio (cmd)"
echo "Serial1: TCP socket on port 4445"

qemu-system-x86_64 \
    -machine pc \
    -cpu max \
    -m 512 \
    -drive file=ubuntu-test.qcow2,format=qcow2,if=virtio \
    -device isa-serial,chardev=charserial0 \
    -chardev stdio,mux=on,id=charserial0,signal=off \
    -device isa-serial,chardev=charserial1 \
    -chardev socket,id=charserial1,port=4445,host=0.0.0.0,server=on,wait=off \
    -device e1000,netdev=net0 \
    -netdev user,id=net0,hostfwd=tcp::2222-:22 \
    -display none

echo "VM killed"