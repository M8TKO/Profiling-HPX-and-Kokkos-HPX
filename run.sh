#!/usr/bin/env bash
set -euo pipefail

OMP_NUM_THREADS=28 \
OMP_PROC_BIND=close \
OMP_PLACES=cores \
./build/rebuildHPXvsMP/cuda/exec_HPXvsMP -t2 --hpx:pu-offset=20 --hpx:pu-step=1