#!/bin/bash

TARGET_APP=$1
OUTPUT_FILE="thread_counts.txt"
SAMPLE_INTERVAL=0.1
NUM_SAMPLES=121

> "$OUTPUT_FILE"

$TARGET_APP &
PID=$!

echo "🔬 Monitoring PID: $PID -> $OUTPUT_FILE (for exactly $NUM_SAMPLES samples)"

for i in $(seq 1 $NUM_SAMPLES)
do
  if ! kill -0 $PID 2>/dev/null; then
    echo "⚠️  Application finished before all samples were taken. Stopping monitor."
    break
  fi
  ps -o nlwp= -p $PID >> "$OUTPUT_FILE"
  sleep $SAMPLE_INTERVAL
done

wait $PID
echo "✅ Monitoring complete."

# ./Shell_Monitoring/run_and_monitor.sh ./build/rebuildHPXvsMP/cuda/exec_HPXvsMP