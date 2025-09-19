#!/bin/bash

TARGET_APP=$1
OUTPUT_FILE="thread_counts.txt"
SAMPLE_INTERVAL=0.1

> "$OUTPUT_FILE"

$TARGET_APP &
PID=$!

echo "🔬 Monitoring PID: $PID -> $OUTPUT_FILE"

while kill -0 $PID 2> /dev/null; do
  ps -o nlwp= -p $PID >> "$OUTPUT_FILE"
  sleep $SAMPLE_INTERVAL
done

wait $PID
echo "✅ Monitoring complete."

# ./Shell_Monitoring/run_and_monitor.sh ./build/rebuildHPXvsMP/cuda/exec_HPXvsMP