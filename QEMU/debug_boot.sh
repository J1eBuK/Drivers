#!/bin/bash
qemu-system-x86_64 \
    -machine pc \
    -cpu max \
    -m 512 \
    -drive file=ubuntu-test.qcow2,format=qcow2,if=virtio \
    -device isa-serial,chardev=charserial0 \
    -chardev stdio,mux=on,id=charserial0,signal=off \
    -append "console=ttyS0,115200n8 earlyprintk=serial debug" \
    -display none \
    -nographic \
    -no-reboot