#!/bin/bash
LOG_FILE="vm_boot.log"
echo "Starting VM, logging to $LOG_FILE..."

# Запуск с простым перенаправлением serial-порта в файл
qemu-system-x86_64 \
    -machine pc \
    -cpu max \
    -m 512 \
    -drive file=ubuntu-test.qcow2,format=qcow2 \
    -serial file:$LOG_FILE \
    -display none \
    -no-reboot

echo ""
echo "VM stopped. Log saved to $LOG_FILE"
echo "=== LOG CONTENT ==="
cat $LOG_FILE