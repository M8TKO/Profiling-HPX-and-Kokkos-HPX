# Kokkos Resources

I will try different ways to manage Cuda streams with Kokkos.

* [MultiGPU Kokkos Support and Stream Managing](https://kokkos.org/kokkos-core-wiki/API/core/MultiGPUSupport.html)
* [Section 3.1, "Execution Spaces"](https://escholarship.org/content/qt0wz9p9vg/qt0wz9p9vg.pdf)
    * > Every parallel operation in Kokkos is enqueued into a first-in, first-out queue in an instance of an execution space (Execution Space Instance). The resources that these instances encapsulate are execution model entities like threads or handles to a device. For example, each Kokkos::Cuda execution space instance encapsulates a CUDA stream, and each Kokkos::Threads instance encapsulates a thread pool.
* [Some syntax on how to possibly spawn different Cuda streams](https://kokkos.org/kokkos-core-wiki/API/core/policies/ExecutionPolicyConcept.html)

So far I have tried these approaches:  
`using DeviceSpace = Kokkos::Cuda;`  
1. Calling the constructor within the Kokkos Kernel call with the idea that it will create a new stream each time.
``` 
Kokkos::parallel_for("process_B", Kokkos::RangePolicy<DeviceSpace>(  DeviceSpace(), 0, N), KOKKOS_LAMBDA(const int i) {
        /* calculation */
    });
```
**Results:** nsys showed the same stream being used

2. Each time declaring a new variable of the type `DeviceSpace` and forwarding it to the Cuda Kernel.
``` 
DeviceSpace deviceSpace;  

Kokkos::parallel_for("process_B", Kokkos::RangePolicy<DeviceSpace>(  deviceSpace, 0, N), KOKKOS_LAMBDA(const int i) {
        /* calculation */
    });
```
**Results:** nsys showed the same stream being used

3. Because the previous tries didn't work, I suspected it may have been a scope problem because the kernel calling thread exited the function - at this point there was no `Kokkos::fence()` in between the end on the kernel call the the exit point of the function. So I made a global variable:
`std::vector<DeviceSpace> deviceSpaces;`
``` 
deviceSpaces.push_back(DeviceSpace());  

Kokkos::parallel_for("process_B", Kokkos::RangePolicy<DeviceSpace>(  deviceSpaces[deviceSpaces.size()-1], 0, N), KOKKOS_LAMBDA(const int i) {
        /* calculation */
    });
```
**Results:** nsys showed the same stream being used

4. More interesting try. 
```
    cudaStream_t stream;
    cudaStreamCreate(&stream);
    Kokkos::Cuda exec_space(stream);

    Kokkos::parallel_for("process_B", Kokkos::RangePolicy<DeviceSpace>(  exec_space, 0, N), KOKKOS_LAMBDA(const int i) {
        /* calculation */
    });
    Kokkos::fence();

```
**Results:** The function was ran 3 times == 3 HPX tasks. Total of 5 streams were spawned, 1 defalt and streams 13,14,15,16. Streams 14,15,16 each called the kernel once, stream 13 had less then 0.1% GPU work, it called Memcpy and Memset both 2 times, that might have been the thread that copies the View from host to device and device to host. Currently there is a `Kokkos::fence()` after the Kokkos kernel call so next try will be the without it. 

5. Instead of using `std::vector<hpx::futures<void>>`, I tried  
```
 hpx::for_each(
            hpx::execution::par,  // The parallel execution policy
            iterations.begin(),             // Start of the range to iterate over
            iterations.end(),               // End of the range
            [&](int i) {                    // The loop body (lambda function)
                Kokkos::View<double*, HostSpace> private_data("private_data_" + std::to_string(i), N);
                process_kernel_B(private_data, N);
            }
        );
```
**Results:** Same problem, there are multiple streams but they are sequential. `Kokkos::fence()` did not have an impact.
