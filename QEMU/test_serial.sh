#!/bin/bash
echo "=== SERIAL PORT TEST ==="
echo "Sending messages..."

while true; do
    echo "HOST: $(date '+%H:%M:%S')" | nc localhost 4444
    sleep 1
    # Проверяем, работает ли соединение
    if ! nc -z localhost 4444; then
        echo "Connection lost, restaring..."
        sleep 2
    fi
done