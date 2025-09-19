#include <hpx/init.hpp>
#include <hpx/future.hpp>
#include <hpx/algorithm.hpp>
#include <cstdint>
#include <Kokkos_Core.hpp>
#include <iostream>
#include <cmath>
#include <fstream>
#include <list>
#include <vector>

using HostSpace = Kokkos::DefaultHostExecutionSpace;
using DeviceSpace = Kokkos::Cuda;

void process_kernel_A(Kokkos::View<double*, HostSpace> data, int N) {
    Kokkos::parallel_for("process_A", Kokkos::RangePolicy<HostSpace>(0, N), KOKKOS_LAMBDA(const int i) {
        double temp_val = static_cast<double>(i);
        for (int k = 0; k < 1000; ++k) {
            temp_val = std::sin(0.001 * temp_val);
        }
        data(i) = temp_val;
    });
    //std::cout << "Fencing of process A" << std::endl;
    Kokkos::fence();
}

void process_kernel_B(DeviceSpace partitionSpace, int N, int index) {

    Kokkos::parallel_for(
        "process_B"+ std::to_string(index), 
        Kokkos::RangePolicy<DeviceSpace>(  partitionSpace, 0, N), 
        KOKKOS_LAMBDA(const int i) {
            volatile double temp_val = static_cast<double>(N - i);
            for (int k = 0; k < 1e4; ++k)
                temp_val = cos(0.001 * temp_val);
        }
    );
    Kokkos::fence();
}

void process_kernel_C(Kokkos::View<double*, HostSpace> data, int N) {
    Kokkos::parallel_for("process_C", Kokkos::RangePolicy<HostSpace>(0, N), KOKKOS_LAMBDA(const int i) {
        double temp_val = static_cast<double>(i * i);
        for (int k = 0; k < 1000; ++k) {
            temp_val = std::tanh(0.0001 * temp_val);
        }
        data(i) = temp_val;
    });
    std::cout << "Fencing of process C" << std::endl;
    Kokkos::fence();
}

int hpx_main(int argc, char* argv[]) {
    std::cout << "Running on: " << HostSpace::name() << "\n";

    std::ofstream outfile("results.txt");
    outfile << "ExecutionSpace: " << HostSpace::name() << std::endl;
    
    const int N = 1e5;
    for (int num_futures = 3; num_futures <= 3; ++num_futures) {
        
        
        Kokkos::Timer timer;
       
        auto partitionSpaces = Kokkos::Experimental::partition_space(DeviceSpace(), std::vector<int>(num_futures, 1));
        
        std::vector<hpx::future<void>> futures;
        futures.reserve(num_futures);
        for (int i = 0; i < num_futures; ++i) {
            futures.push_back(
                hpx::async( [=, &partitionSpaces]() {
                    process_kernel_B(partitionSpaces[i], N, i);
                })
            );
        }
        hpx::wait_all(futures);         

        double time = timer.seconds();
        std::cout << "Iteration with " << num_futures << " futures took " << time << " seconds." << std::endl;
        outfile << num_futures << ", " << time << std::endl;
    }
    
    outfile.close();
    Kokkos::finalize();

   if( HostSpace::name() != "HPX" )
        return hpx::local::finalize();
    else
        return 0;
}

int main(int argc, char* argv[]) {
    Kokkos::initialize(argc, argv);

    if( HostSpace::name() != "HPX" )
        return hpx::local::init(hpx_main, argc, argv);
    else
        return hpx_main(argc, argv);
}