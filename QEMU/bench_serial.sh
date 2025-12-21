#!/bin/bash
echo "=== SERIAL PORT BENCHMARK ==="
echo "Starting listener on port 4445..."
nc -l -p 4445 > bench_results.txt &
NC_PID=$!
echo "Listener PID: $NC_PID"

sleep 2

echo "Sending test data..."
echo "BENCHMARK_START $(date)" | nc localhost 4445
dd if=/dev/urandom bs=1K count=100 2>/dev/null | nc localhost 4445
echo "BENCHMARK_END $(date)" | nc localhost 4445

sleep 3

echo "Stopping listener..."
kill $NC_PID 2>/dev/null

echo ""
echo "Benchmark results saved to bench_results.txt"
echo "SCREENSHOT: Make screenshot of bench_results.txt content"
cat bench_results.txt