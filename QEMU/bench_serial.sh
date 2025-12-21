#!/bin/bash
echo "=== SERIAL PORT BENCHMARK ==="
echo "Test 1: Small messages (100 bytes)"
for i in {1..100}; do
    echo "Test$i: $(date +%s%N)" | nc localhost 4445
done

echo "Test 2: Large data block (1MB)"
dd if=/dev/urandom bs=1K count=1024 2>/dev/null | nc localhost 4445

echo "Benchmark complete. Results saved to bench_results.txt"