#!/bin/bash

RESULTS_DIR="./results"
CSV_FILE="benchmarks_data.csv"

if [ ! -d "$RESULTS_DIR" ]; then
  echo "Error: Results directory '$RESULTS_DIR' not found."
  exit 1
fi

header="Config,nsets,bsize,assoc,repl"
benchmarks=$(ls -1 "$RESULTS_DIR" | sed -n 's/result_\([^_]*\)_.*/\1/p' | sort -u)
for bench in $benchmarks; do
  header+=",${bench}_il1_hit_rate,${bench}_dl1_hit_rate"
done
echo "$header" > "$CSV_FILE"

temp_file=$(mktemp)
echo "Processing result files..."

for result_file in "$RESULTS_DIR"/*; do
  filename=$(basename "$result_file")
  
  benchmark_name=$(echo "$filename" | sed -n 's/result_\([^_]*\)_.*/\1/p')
  config_name=$(echo "$filename" | sed -n 's/result_[^_]*_\([^_]*\.txt\)_.*/\1/p')
  prefetcher_name=$(echo "$filename" | sed -n 's/.*\.txt_\(.*\)\.txt/\1/p')
  
  config_key="${config_name}_${prefetcher_name}"

  il1_config_line=$(grep "^-cache:il1" "$result_file")
  il1_config=$(echo "$il1_config_line" | sed -n 's/.* \([^ ]*\) # l1 inst cache.*/\1/p')

  # Safely parse parameters; default to NA if missing
  if [ -z "$il1_config" ]; then
    nsets="NA"; bsize="NA"; assoc="NA"; repl="NA"
  else
    nsets=$(echo "$il1_config" | cut -d':' -f2)
    bsize=$(echo "$il1_config" | cut -d':' -f3)
    assoc=$(echo "$il1_config" | cut -d':' -f4)
    repl=$(echo "$il1_config" | cut -d':' -f5)
  fi

  il1_miss_rate=$(grep "il1.miss_rate" "$result_file" | awk '{print $2}')
  dl1_miss_rate=$(grep "dl1.miss_rate" "$result_file" | awk '{print $2}')

  # Validate numbers before bc call or assign NA
  if [[ $il1_miss_rate =~ ^[0-9]*\.?[0-9]+$ ]]; then
    il1_hit_rate=$(echo "scale=4; 1 - $il1_miss_rate" | bc)
  else
    il1_hit_rate="NA"
  fi
  if [[ $dl1_miss_rate =~ ^[0-9]*\.?[0-9]+$ ]]; then
    dl1_hit_rate=$(echo "scale=4; 1 - $dl1_miss_rate" | bc)
  else
    dl1_hit_rate="NA"
  fi

  echo "${config_key},${nsets},${bsize},${assoc},${repl},${benchmark_name},${il1_hit_rate},${dl1_hit_rate}" >> "$temp_file"
done

unique_configs=$(cut -d',' -f1 "$temp_file" | sort -u)

for config in $unique_configs; do
  row_prefix=$(grep "^${config}," "$temp_file" | head -1 | cut -d',' -f1-5)
  csv_row="$row_prefix"

  for bench in $benchmarks; do
    il1_hr=$(grep "^${config},.*,${bench}," "$temp_file" | cut -d',' -f7)
    dl1_hr=$(grep "^${config},.*,${bench}," "$temp_file" | cut -d',' -f8)
    csv_row+=",${il1_hr:-NA},${dl1_hr:-NA}"
  done

  echo "$csv_row" >> "$CSV_FILE"
done

rm "$temp_file"
echo "Processing complete. Summary report saved to $CSV_FILE"
