#!/bin/bash

echo "Installing packages..."
sudo apt update && sudo apt install -y netcat-openbsd socat qemu-system-x86

echo "Making test package..."
touch serial_output.txt

echo "Checking for VM image..."
if [ -f "ubuntu-test.qcow2" ]; then
    echo "Image ubuntu-test.qcow2 not found"
    ls -la ubuntu-test.qcow2
else
    echo "Error: file ubuntu-test.qcow2 not found!"
    exit 1
fi

echo "Setting up..."
chmod +x start_vm.sh test_serial.sh

echo ""
echo "Done!"