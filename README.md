### **HPX vs. OpenMP Performance Results**

| Test Scenario | HPX Time (s) | OpenMP Time (s) | **Winner** |
| :--- | :---: | :---: | :---: |
| **10 Concurrent Futures** | 60.89 | 64.62 | **HPX** |
| **50 Concurrent Futures** | 325.72 | 469.92 | **HPX** |

# Performance Results: HPX vs. OpenMP

This document presents the benchmark results for concurrently executing a variable number of compute kernels using the HPX and OpenMP parallel runtimes.

***

### Performance Visualization

![Performance Comparison: HPX vs. OpenMP](hpx_vs_omp_performance.png)

As you can see, the execution time for HPX scales in a nearly linear fashion as the number of concurrent kernels increases. In contrast, the execution time for OpenMP shows a much steeper increase, indicating that HPX is significantly more efficient at handling this type of highly concurrent workload.

***

### Raw Data Comparison

The table below shows the total time in seconds required to execute a given number of concurrent kernels.

| Number of Kernels | HPX Time (s) | OpenMP Time (s) |
| :---: | :---: | :---: |
| 10 | 0.356 | 0.530 |
| 11 | 0.354 | 0.929 |
| 12 | 0.386 | 1.396 |
| 13 | 0.417 | 2.322 |
| 14 | 0.448 | 2.667 |
| 15 | 0.480 | 3.486 |
| 16 | 0.512 | 3.386 |
| 17 | 0.544 | 3.849 |
| 18 | 0.576 | 3.955 |
| 19 | 0.608 | 4.613 |
| 20 | 0.641 | 4.239 |
| 21 | 0.672 | 4.812 |
| 22 | 0.774 | 5.340 |
| 23 | 0.927 | 5.266 |
| 24 | 0.949 | 5.259 |
| 25 | 1.003 | 5.526 |
| 26 | 1.035 | 5.580 |
| 27 | 1.077 | 5.889 |
| 28 | 1.115 | 6.451 |
| 29 | 1.154 | 6.584 |
| 30 | 1.201 | 6.915 |
| 31 | 1.268 | 7.036 |
| 32 | 1.289 | 7.159 |
| 33 | 1.318 | 7.570 |
| 34 | 1.356 | 7.498 |
| 35 | 1.397 | 7.508 |
| 36 | 1.425 | 8.088 |
| 37 | 1.476 | 8.596 |
| 38 | 1.513 | 8.451 |
| 39 | 1.557 | 8.861 |
| 40 | 1.588 | 9.051 |
| 41 | 1.641 | 9.321 |
| 42 | 1.702 | 9.347 |
| 43 | 1.735 | 9.649 |
| 44 | 1.776 | 10.211 |
| 45 | 1.794 | 9.696 |
| 46 | 1.835 | 9.519 |
| 47 | 1.884 | 10.456 |
| 48 | 1.904 | 10.352 |
| 49 | 1.959 | 10.929 |
| 50 | 1.987 | 10.870 |

```mermaid
xychart-beta
    title "Performance Comparison: HPX vs. OpenMP"
    x-axis "Number of Concurrent Kernels" [10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50]
    y-axis "Execution Time (s)"
    line "HPX" [0.356, 0.354, 0.386, 0.417, 0.448, 0.480, 0.512, 0.544, 0.576, 0.608, 0.641, 0.672, 0.774, 0.927, 0.949, 1.003, 1.035, 1.077, 1.115, 1.154, 1.201, 1.268, 1.289, 1.318, 1.356, 1.397, 1.425, 1.476, 1.513, 1.557, 1.588, 1.641, 1.702, 1.735, 1.776, 1.794, 1.835, 1.884, 1.904, 1.959, 1.987]
    line "OpenMP" [0.530, 0.929, 1.396, 2.322, 2.667, 3.486, 3.386, 3.849, 3.955, 4.613, 4.239, 4.812, 5.340, 5.266, 5.259, 5.526, 5.580, 5.889, 6.451, 6.584, 6.915, 7.036, 7.159, 7.570, 7.498, 7.508, 8.088, 8.596, 8.451, 8.861, 9.051, 9.321, 9.347, 9.649, 10.211, 9.696, 9.519, 10.456, 10.352, 10.929, 10.870]
```



For `num_futures = 10` and `N=1e5`, running one CPU kernel and one GPU kernel:
- if the backend is **OpenMP**, there are 109 `pthread_create` calls
- if the back is **HPX**, there are  28 `pthread_create` calls

# Analysis of Thread Creation during Program Startup

A total of **28** OS-level threads (`pthreads`) were created during the program's lifecycle. These can be grouped into four distinct phases. More details in **pthread_calls_HPXschedulingHPXhostBackendCUDAdeviceBackend.log**

-----

### Breakdown of Thread Creation Events

  * **HPX Runtime Initialization (Threads 1-4)**

      * These first threads are created by the HPX runtime to manage core services, such as I/O and the main thread manager, during the initial startup phase.

    <!-- end list -->

    ```gdb
     4--- pthread_create called from: ---
    #2  hpx::util::io_service_pool::run_locked(...)
    #3  hpx::util::io_service_pool::run(...)
    #4  hpx::threads::threadmanager::run()
    #5  hpx::runtime_distributed::start(...)
    #17 main(...)
    ```

  * **HPX Worker Thread Pool (Threads 5-24)**

      * This is the main pool of HPX worker threads. These threads are responsible for executing all the parallel tasks in your application (e.g., `hpx::async` and the Kokkos kernels).

    <!-- end list -->

    ```gdb
     5--- pthread_create called from: ---
    #2  hpx::threads::detail::scheduled_thread_pool<...>::add_processing_unit_internal(...)
    #4  hpx::threads::threadmanager::run()
    #5  hpx::runtime_distributed::start(...)
    #17 main(...)
    ```

  * **CUDA Driver Initialization (Threads 25-27)**

      * The CUDA runtime creates its own internal helper threads when it's initialized by Kokkos. These manage GPU communication and context.

    <!-- end list -->

    ```gdb
     25--- pthread_create called from: ---
    #5  cuInit () from /lib/x86_64-linux-gnu/libcuda.so.1
    #12 cudaGetDeviceCount () from /usr/local/cuda-13.0/lib64/libcudart.so.13
    #15 Kokkos::Cuda::impl_initialize (...)
    #21 main (...)
    ```

  * **HPX Shutdown (Thread 28)**

      * A final thread is created during the finalization phase to help manage the clean shutdown and synchronization of the HPX runtime.

    <!-- end list -->

    ```gdb
     28--- pthread_create called from: ---
    #2  hpx::runtime_distributed::wait()
    #3  hpx::local::stop(...)
    #4  Kokkos::Experimental::HPX::impl_finalize ()
    #7  Kokkos::finalize ()
    #8  hpx_main (...)
    #9  main (...)
    ```
