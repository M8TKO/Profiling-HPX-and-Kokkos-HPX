This is a test for 3 kernels.

./build/rebuildHPXvsMP/cuda/exec_HPXvsMP --hpx:threads=all
Running on: HPX
Scatter kernel complete.
Reduce kernel complete.
Fill kernel complete.
Kernels took 48.6561 seconds.

OMP_PROC_BIND=spread OMP_PLACES=threads  ./build/rebuildHPXvsMP/cuda/exec_HPXvsMP 
Running on: OpenMP
Reduce kernel complete.
Scatter kernel complete.
Fill kernel complete.
Kernels took 37.4085 seconds.

OMP_PROC_BIND=spread OMP_PLACES=threads  ./build/rebuildHPXvsMP/cuda/exec_HPXvsMP --hpx:threads=all
Running on: OpenMP
Scatter kernel complete.
Reduce kernel complete.
Fill kernel complete.
Kernels took 41.8996 seconds.