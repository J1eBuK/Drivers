#!/bin/bash
echo "=== UART EMULATION TEST ==="
echo "RX: Host → Guest (port 4444)"
echo "TX: Guest → Host (port 4445)"
echo ""

# Start TX receiver (Guest → Host)
echo "Starting TX receiver (port 4445)..."
nc -l -p 4445 > guest_tx.txt &
TX_PID=$!
echo "TX receiver PID: $TX_PID"

# Function to cleanup
cleanup() {
    kill $TX_PID 2>/dev/null
    echo "Test stopped. Results saved to guest_tx.txt"
    exit 0
}
trap cleanup INT TERM

# RX transmitter (Host → Guest)
echo ""
echo "Starting RX transmitter (port 4444)..."
echo "Press Ctrl+C to stop"
echo ""

while true; do
    echo "HOST-RX: $(date '+%T')" | nc localhost 4444
    sleep 1
done