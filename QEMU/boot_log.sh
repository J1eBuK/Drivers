#!/bin/bash
LOG_FILE="ubuntu_boot.log"
echo "Starting Ubuntu VM with serial logging to $LOG_FILE"

qemu-system-x86_64 \
    -machine pc \
    -cpu max \
    -m 1024 \
    -drive file=ubuntu-test.qcow2,format=qcow2,if=virtio \
    -device isa-serial,chardev=serial0 \
    -chardev file,path=$LOG_FILE,id=serial0 \
    -append "console=ttyS0,115200n8" \
    -display none \
    -no-reboot

echo "VM stopped. Log saved to $LOG_FILE"
echo "=== LOG CONTENT ==="
tail -30 $LOG_FILE