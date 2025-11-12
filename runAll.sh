#!/bin/bash

chmod +x run_benchmarks.sh
chmod +x extract_data.sh

./run_benchmarks.sh
./extract_data.sh
python3 plot_results.py