#!/bin/bash
echo "Serial port performance test"
echo "Listening on port 4445... (background)"
nc -l -p 4445 > serial_output.txt &
NC_PID=$!
echo "Netcat PID: $NC_PID"

echo "Waiting 2 seconds for netcat startup..."
sleep 2

echo "Sending test messages..."
for i in {1..5}; do
    echo "Message $i from host: $(date)" | nc localhost 4445
    sleep 1
done

echo "Waiting for VM response (5 seconds)..."
sleep 5

echo "Stopping netcat..."
kill $NC_PID 2>/dev/null

echo ""
echo "=== TEST RESULTS ==="
echo "Received data:"
cat serial_output.txt

echo ""
echo "File saved: serial_output.txt"
echo "SCREENSHOT: Make screenshot of this terminal with results"