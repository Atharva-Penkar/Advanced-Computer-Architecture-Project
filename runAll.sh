#!/bin/bash

chmod +x run_benchmarks.sh
chmod +x extract_data.sh

./run_benchmarks.sh
./extract_data.sh

VENV_DIR="aca_plot_venv"

if [ ! -d "$VENV_DIR" ]; then
  python3 -m venv "$VENV_DIR"
fi

source "$VENV_DIR/bin/activate"

pip install --upgrade pip
pip install matplotlib pandas numpy

python3 plotdata.py
deactivate
