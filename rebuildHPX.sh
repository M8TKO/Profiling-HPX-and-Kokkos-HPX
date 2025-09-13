#!/usr/bin/env bash
set -euo pipefail
clear

SCRIPT_PATH="$(readlink -f "$0")"
SOURCEDIR="$(dirname "$SCRIPT_PATH")"
TARGET="$(basename "$SCRIPT_PATH" .sh)"

# --- MODIFIED: Removed unused CUDA_PREFIX variable ---
HPX_PREFIX="$HOME/hpx-gcc11-nordtsc"
BOOST_ROOT="/home/petricic1/spack/opt/spack/linux-skylake/boost-1.74.0-xhyvxpzunv4gdvj7glc66e2uqdpf2xud"

FC_BASE="$HOME/.fc"
STAGE1="$SOURCEDIR/build/${TARGET}/fc_bootstrap"
BUILDDIR="$SOURCEDIR/build/${TARGET}/hpx_only" # Changed build dir name for clarity

export BOOST_ROOT
# --- MODIFIED: Removed CUDA-related paths ---
export LD_LIBRARY_PATH="$BOOST_ROOT/lib:${LD_LIBRARY_PATH:-}"
unset CC CXX CPATH CPLUS_INCLUDE_PATH INCLUDE

rm -rf "$STAGE1" "$BUILDDIR"
mkdir -p "$STAGE1" "$FC_BASE"

# This first stage for fetching content can remain the same
cmake -S "$SOURCEDIR" -B "$STAGE1" \
  -DFETCHCONTENT_BASE_DIR="$FC_BASE" \
  -DFETCHCONTENT_UPDATES_DISCONNECTED=OFF \
  -DHPX_DIR="$HPX_PREFIX/lib/cmake/HPX" \
  -DCMAKE_PREFIX_PATH="$HPX_PREFIX;$BOOST_ROOT" \
  -DBOOST_ROOT="$BOOST_ROOT" \
  -DBUILD_TESTING=OFF

# --- MODIFIED: Removed the nvcc_wrapper variable ---

# This is the main configuration step with changes
cmake --fresh -S "$SOURCEDIR" -B "$BUILDDIR" "$@" \
  -DCMAKE_BUILD_TYPE=Debug \
  -DFETCHCONTENT_BASE_DIR="$FC_BASE" \
  -DFETCHCONTENT_UPDATES_DISCONNECTED=OFF \
  -DHPX_DIR="$HPX_PREFIX/lib/cmake/HPX" \
  -DCMAKE_PREFIX_PATH="$HPX_PREFIX;$BOOST_ROOT" \
  -DBOOST_ROOT="$BOOST_ROOT" \
  -DBUILD_TESTING=OFF \
  -DKokkos_ENABLE_HPX=ON \
  -DKokkos_ENABLE_SERIAL=ON \
  -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
  -DCMAKE_CXX_FLAGS="-isystem $BOOST_ROOT/include -w"
# --- MODIFIED: Removed all CUDA-specific CMake variables ---
# -DCMAKE_CXX_COMPILER="$WRAPPER"
# -DCMAKE_CUDA_COMPILER="$CUDA_PREFIX/bin/nvcc"
# -DCUDAToolkit_ROOT="$CUDA_PREFIX"
# -DKokkos_ARCH_AMPERE86=ON
# -DKokkos_ENABLE_CUDA_LAMBDA=ON
# -DKokkos_ENABLE_CUDA_GRAPH=OFF

cmake --build "$BUILDDIR" -j16