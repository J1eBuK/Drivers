#!/bin/bash
echo "=== SERIAL PORT TEST ==="
echo "Step 1: Start message receiver (guest -> host)"
nc -l -p 4445 > guest_messages.txt &
RECEIVER_PID=$!
echo "Receiver started (PID: $RECEIVER_PID)"

echo ""
echo "Step 2: Send messages from host to guest"
for i in {1..3}; do
    echo "HOST: Test $i at $(date)" | nc localhost 4444
    echo "Sent message $i to guest"
    sleep 1
done

echo ""
echo "Step 3: Wait for guest response (5 seconds)"
sleep 5

echo ""
echo "Step 4: Stop receiver"
kill $RECEIVER_PID 2>/dev/null

echo ""
echo "=== TEST RESULTS ==="
echo "Messages received FROM guest:"
cat guest_messages.txt

echo ""
echo "Test complete. Make screenshot of this output."