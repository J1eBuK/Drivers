#!/bin/bash

echo "Starting Alpine VM with 2 serial-ports..."
echo "Serial0: stdio (cmd)"
echo "Serial1: TCP socket on port 4444/4445"

qemu-system-x86_64 \
    -machine pc \
    -cpu max \
    -m 512 \
    -drive file=ubuntu-test.qcow2,format=qcow2,if=virtio \
    -device isa-serial,chardev=console \
    -chardev stdio,mux=on,id=console,signal=off \
    -device isa-serial,chardev=uart_rx \
    -chardev socket,id=uart_rx,port=4444,host=0.0.0.0,server=on,wait=off \
    -device isa-serial,chardev=uart_tx \
    -chardev socket,id=uart_tx,port=4445,host=0.0.0.0,server=on,wait=off \
    -display none

echo "VM killed"