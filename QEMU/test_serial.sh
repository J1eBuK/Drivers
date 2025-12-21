#!/bin/bash
# Скрипт для тестирования обмена данными через serial-порт
# Сохранить как test_serial.sh

echo "Data transmitting test through serial-port (port 4445)"
echo "Starting netcat for listening to 4445..."
nc -l -p 4445 > serial_output.txt &

NC_PID=$!
echo "PID netcat: $NC_PID"

echo "Wait for 2 seconds to start netcat..."
sleep 2

echo "Sending test package..."
echo "Hello from Dudka at $(date)" | nc localhost 4445

echo "Waiting for answer..."
sleep 5

echo "Stopping netcat..."
kill $NC_PID 2>/dev/null

echo "Received data:"
cat serial_output.txt

echo ""
echo "Vot tut nado ne zabit sdelat screenshot"
echo "File: serial_output.txt saved"