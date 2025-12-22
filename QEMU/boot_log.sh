#!/bin/bash
LOG_FILE="vm_boot.log"
echo "Starting VM with serial logging to $LOG_FILE"

qemu-system-x86_64 \
    -machine pc \
    -cpu max \
    -m 512 \
    -drive file=ubuntu-test.qcow2,format=qcow2,if=virtio \
    -device isa-serial,chardev=serial0 \
    -chardev file,path=$LOG_FILE,id=serial0 \
    -display none \
    -no-reboot

echo "VM stopped. Log saved to $LOG_FILE"
echo "=== LOG CONTENT ==="
tail -20 $LOG_FILE