#!/bin/bash

SIM_CACHE_EXEC="./simplescalar/simplesim-3.0/sim-cache"
BENCHMARK_DIR="./benchmarks"
CONFIG_DIR="./cache_configs"
RESULTS_DIR="./results"

PREFETCHER_TYPES=("none" "next_line" "stride")
MAX_JOBS=24  # Max number of concurrent jobs

echo "Starting cache simulation for all benchmarks and configurations..."

job_count=0

for benchmark_path in "$BENCHMARK_DIR"/*; do
  benchmark_name=$(basename "$benchmark_path")

  for config_path in "$CONFIG_DIR"/*; do
    config_name=$(basename "$config_path")

    for prefetcher in "${PREFETCHER_TYPES[@]}"; do

      echo "Running: Benchmark=$benchmark_name, Config=$config_name, Prefetcher=$prefetcher"

      output_file="${RESULTS_DIR}/result_${benchmark_name}_${config_name}_${prefetcher}.txt"

      # Run sim-cache in background
      "$SIM_CACHE_EXEC" -prefetcher "$prefetcher" -config "$config_path" \
        "$benchmark_path" > "$output_file" 2>&1 &

      ((job_count++))

      # If max jobs reached, wait for any to finish
      if (( job_count >= MAX_JOBS )); then
        wait -n  # wait for any background job to finish
        ((job_count--))
      fi

    done
  done
done

# Wait for all remaining background jobs to finish
wait

echo "All simulations complete."
