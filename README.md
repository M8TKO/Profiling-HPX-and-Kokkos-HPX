## Benchmark Results: Maximum Effective Bandwidth (GB/s) 

| Kernel Type | OpenMP + CUDA | HPX + CUDA |
| :--- | :---: | :---: |
| **Tasking** | 1.45 GB/s | **7.37 GB/s** |
| **Total Kernel** | 23.40 GB/s | 22.14 GB/s |
| **Serial Kernels** | 13.38 GB/s | 13.14 GB/s |

-----

## HPX Tasking and Kokkos HPX Backend  
We are suspecting that `hpx::dataflow` might spawn each thread for each call (or sometimes at least).


The following code creates 100 sequential tasks. Using GDB, 25 `pthread_create` calls were made. New OS threads are not spawned for each task.

```cpp
int fun(int x) {
    // Simulate work
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    return x + 1;
}

// --- In main ---
int temp = 0;
hpx::future<int> f = hpx::make_ready_future(temp);

for (int i = 0; i < 100; i++) {
    f = hpx::dataflow(hpx::unwrapping(fun), f);
}

f.get();
```



##  Performance Comparison: `hpx::dataflow` vs. Continuations

This table compares the maximum effective bandwidth of the application when using `hpx::dataflow` versus manually chaining tasks with continuations like `hpx::async` and `.then()`.

| Tasking Approach | Host Execution Space | Device Execution Space | Max. Effective Bandwidth (GB/sec) |
| :--- | :--- | :--- | :--- |
| **Using `hpx::dataflow`** | `OpenMP` | `Cuda` | **`1.45`** |
| **`async` & `.then`** | `OpenMP` | `Cuda` | `1.42446` |

