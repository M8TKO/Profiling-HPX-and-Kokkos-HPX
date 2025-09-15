#include <hpx/init.hpp>
#include <hpx/future.hpp>
#include <cstdint>
#include <Kokkos_Core.hpp>
#include <iostream>
#include <cmath>
#include <fstream>
#include <vector>

using HostSpace = Kokkos::DefaultHostExecutionSpace;

void process_kernel_A(Kokkos::View<double*, HostSpace> data, int N) {
    Kokkos::parallel_for("process_A", Kokkos::RangePolicy<HostSpace>(0, N), KOKKOS_LAMBDA(const int i) {
        double temp_val = static_cast<double>(i);
        for (int k = 0; k < 1000; ++k) {
            temp_val = std::sin(0.001 * temp_val);
        }
        data(i) = temp_val;
    });
    std::cout << "Fencing of process A" << std::endl;
    Kokkos::fence();
}

void process_kernel_B(Kokkos::View<double*, HostSpace> data, int N) {
    Kokkos::parallel_for("process_B", Kokkos::RangePolicy<HostSpace>(0, N), KOKKOS_LAMBDA(const int i) {
        double temp_val = static_cast<double>(N - i);
        for (int k = 0; k < 1000; ++k) {
            temp_val = std::cos(0.001 * temp_val);
        }
        data(i) = temp_val;
    });
    std::cout << "Fencing of process B" << std::endl;
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
    Kokkos::initialize(argc, argv);
    std::cout << "Running on: " << HostSpace::name() << "\n";

    std::ofstream outfile("results.txt");
    outfile << "ExecutionSpace: " << HostSpace::name() << std::endl;

    for (int num_futures = 2; num_futures <= 50; ++num_futures) {
        const int N = 1e3;
        Kokkos::Timer timer;

        std::vector<hpx::future<void>> futures;
        futures.reserve(num_futures);
        for (int i = 0; i < num_futures; i++) {
            Kokkos::View<double*, HostSpace> private_data("private_data_" + std::to_string(i), N);

            switch (i % 3) {
            case 0:
                futures.push_back(hpx::async(process_kernel_A, private_data, N));
                break;
            case 1:
                futures.push_back(hpx::async(process_kernel_B, private_data, N));
                break;
            case 2:
                futures.push_back(hpx::async(process_kernel_C, private_data, N));
                break;
            }
        }

        std::cout << "All " << num_futures << " futures launched concurrently." << std::endl;
        hpx::wait_all(futures);

        double time = timer.seconds();
        std::cout << "Iteration with " << num_futures << " futures took " << time << " seconds." << std::endl;
        outfile << num_futures << ", " << time << std::endl;
    }
    
    outfile.close();
    Kokkos::finalize();

    return hpx::local::finalize();;
}

int main(int argc, char* argv[]) {
    return hpx::init(hpx_main, argc, argv);
}