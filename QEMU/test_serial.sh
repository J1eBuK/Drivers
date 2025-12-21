#!/bin/bash
echo "=== SERIAL PORT TEST ==="
echo "Sending packages to guest"

while true; do
    echo "HOST: $(date '+%H:%M:%S')" | nc localhost 4444
    sleep 1
done