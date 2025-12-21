#!/bin/bash
# Auto-deploy test script
echo "Starting automated deployment test..."
sleep 2
echo "INSTALL_KERNEL" > /dev/ttyS1
sleep 1
echo "CONFIGURE_NETWORK eth0 dhcp" > /dev/ttyS1
sleep 1
echo "SETUP_SSH" > /dev/ttyS1
echo "Deployment commands sent"