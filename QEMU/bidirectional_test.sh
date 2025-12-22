#!/bin/bash
echo "=== BIDIRECTIONAL SERIAL TEST ==="

# Start receiver for guest messages (background)
echo "Starting receiver for guest messages (port 4445)..."
nc -l -p 4445 > guest_messages.txt &
RECEIVER_PID=$!
echo "Receiver PID: $RECEIVER_PID"

# Start continuous sending to guest
echo ""
echo "Starting continuous sending to guest (port 4444)..."
echo "Press Ctrl+C to stop test"
echo ""

trap 'kill $RECEIVER_PID 2>/dev/null; exit 0' INT

while true; do
    echo "HOST: $(date '+%T')" | nc localhost 4444
    sleep 1
done