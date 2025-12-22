#!/bin/bash
LOG_FILE="vm_boot.log"
echo "Starting VM with detailed logging to $LOG_FILE"

# Запуск с детальным выводом
timeout 90 qemu-system-x86_64 \
    -machine pc \
    -cpu max \
    -m 512 \
    -drive file=detailed_boot.qcow2,format=qcow2,if=virtio \
    -device isa-serial,chardev=serial0 \
    -chardev file,path=$LOG_FILE,id=serial0 \
    -append "console=ttyS0,115200n8 earlyprintk=serial debug loglevel=7" \
    -display none \
    -no-reboot

echo ""
echo "VM stopped after timeout. Log saved to $LOG_FILE"
echo "=== LOG SUMMARY ==="
echo "First 10 lines:"
head -10 $LOG_FILE
echo ""
echo "Last 10 lines:"
tail -10 $LOG_FILE
EOF

chmod +x detailed_log.sh