#!/bin/bash
echo "Starting boot debug on port 4446..."
qemu-system-x86_64 \
    -machine pc \
    -cpu max \
    -m 512 \
    -drive file=ubuntu-test.qcow2,format=qcow2,if=virtio \
    -device isa-serial,chardev=charserial0 \
    -chardev socket,id=charserial0,port=4446,host=0.0.0.0,server=on,wait=off \
    -append "console=ttyS0,115200n8 earlyprintk=serial debug" \
    -display none \
    -nographic \
    -no-reboot