### **HPX vs. OpenMP Performance Results**

| Test Scenario | HPX Time (s) | OpenMP Time (s) | **Winner** |
| :--- | :---: | :---: | :---: |
| **Sequential Kernels** (Fill, Scatter, Reduce) | 48.66 | 37.41 | **OpenMP** |
| **10 Concurrent Futures** | 60.89 | 64.62 | **HPX** |
| **50 Concurrent Futures** | 325.72 | 469.92 | **HPX** |

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

./build/rebuildHPXvsMP/cuda/exec_HPXvsMP --hpx:threads=all
Running on: HPX
Process Kernel A complete.
Process Kernel B complete.
Process Kernel C complete.
Process Kernel A complete.
Process Kernel B complete.
Process Kernel C complete.
Process Kernel A complete.
Process Kernel B complete.
Process Kernel C complete.
All 10 futures launched concurrently.
Process Kernel A complete.
All kernels running in parallel took 60.8891 seconds.

OMP_PROC_BIND=spread OMP_PLACES=threads  ./build/rebuildHPXvsMP/cuda/exec_HPXvsMP --hpx:threads=all
Running on: OpenMP
Process Kernel A complete.
Process Kernel B complete.
Process Kernel C complete.
Process Kernel A complete.
Process Kernel B complete.
Process Kernel C complete.
Process Kernel A complete.
Process Kernel B complete.
All 10 futures launched concurrently.
Process Kernel C complete.
Process Kernel A complete.
All kernels running in parallel took 64.6216 seconds.

OMP_PROC_BIND=spread OMP_PLACES=threads  ./build/rebuildHPXvsMP/cuda/exec_HPXvsMP --hpx:threads=all
Running on: OpenMP
Process Kernel A complete.
Process Kernel C complete.
Process Kernel B complete.
Process Kernel A complete.
Process Kernel B complete.
Process Kernel C complete.
Process Kernel A complete.
Process Kernel B complete.
Process Kernel C complete.
Process Kernel A complete.
Process Kernel B complete.
Process Kernel C complete.
Process Kernel A complete.
Process Kernel B complete.
Process Kernel A complete.
Process Kernel C complete.
Process Kernel B complete.
Process Kernel C complete.
Process Kernel A complete.
Process Kernel B complete.
Process Kernel C complete.
Process Kernel A complete.
Process Kernel B complete.
Process Kernel C complete.
Process Kernel A complete.
Process Kernel B complete.
Process Kernel C complete.
Process Kernel A complete.
Process Kernel B complete.
Process Kernel C complete.
Process Kernel A complete.
Process Kernel B complete.
Process Kernel C complete.
Process Kernel A complete.
Process Kernel B complete.
Process Kernel C complete.
Process Kernel A complete.
Process Kernel B complete.
Process Kernel C complete.
Process Kernel A complete.
Process Kernel B complete.
Process Kernel C complete.
Process Kernel A complete.
Process Kernel B complete.
Process Kernel C complete.
Process Kernel A complete.
Process Kernel B complete.
All 50 futures launched concurrently.
Process Kernel C complete.
Process Kernel B complete.
Process Kernel A complete.
All kernels running in parallel took 469.915 seconds.

OMP_PROC_BIND=spread OMP_PLACES=threads  ./build/rebuildHPXvsMP/cuda/exec_HPXvsMP --hpx:threads=all
Running on: HPX
Process Kernel A complete.
Process Kernel B complete.
Process Kernel C complete.
Process Kernel A complete.
Process Kernel B complete.
Process Kernel C complete.
Process Kernel A complete.
Process Kernel B complete.
Process Kernel C complete.
Process Kernel A complete.
Process Kernel B complete.
Process Kernel C complete.
Process Kernel A complete.
Process Kernel B complete.
Process Kernel C complete.
Process Kernel A complete.
Process Kernel B complete.
Process Kernel C complete.
Process Kernel A complete.
Process Kernel B complete.
Process Kernel C complete.
Process Kernel A complete.
Process Kernel B complete.
Process Kernel C complete.
Process Kernel A complete.
Process Kernel B complete.
Process Kernel C complete.
Process Kernel A complete.
Process Kernel B complete.
Process Kernel C complete.
Process Kernel A complete.
Process Kernel B complete.
Process Kernel C complete.
Process Kernel A complete.
Process Kernel B complete.
Process Kernel C complete.
Process Kernel A complete.
Process Kernel B complete.
Process Kernel C complete.
Process Kernel A complete.
Process Kernel B complete.
Process Kernel C complete.
Process Kernel A complete.
Process Kernel B complete.
Process Kernel C complete.
Process Kernel A complete.
Process Kernel B complete.
Process Kernel C complete.
Process Kernel A complete.
All 50 futures launched concurrently.
Process Kernel B complete.
All kernels running in parallel took 325.724 seconds.