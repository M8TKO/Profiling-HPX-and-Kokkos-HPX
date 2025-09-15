#include <hpx/init.hpp>
#include <hpx/future.hpp>
#include <cstdint>
#include <Kokkos_Core.hpp>
#include <iostream>
#include <cmath>
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
    Kokkos::fence();
    std::cout << "Process Kernel A complete." << std::endl;
}

void process_kernel_B(Kokkos::View<double*, HostSpace> data, int N) {
    Kokkos::parallel_for("process_B", Kokkos::RangePolicy<HostSpace>(0, N), KOKKOS_LAMBDA(const int i) {
        double temp_val = static_cast<double>(N - i);
        for (int k = 0; k < 1000; ++k) {
            temp_val = std::cos(0.001 * temp_val);
        }
        data(i) = temp_val;
    });
    Kokkos::fence();
    std::cout << "Process Kernel B complete." << std::endl;
}

void process_kernel_C(Kokkos::View<double*, HostSpace> data, int N) {
    Kokkos::parallel_for("process_C", Kokkos::RangePolicy<HostSpace>(0, N), KOKKOS_LAMBDA(const int i) {
        double temp_val = static_cast<double>(i * i);
        for (int k = 0; k < 1000; ++k) {
            temp_val = std::tanh(0.0001 * temp_val);
        }
        data(i) = temp_val;
    });
    Kokkos::fence();
    std::cout << "Process Kernel C complete." << std::endl;
}

int hpx_main(int argc, char* argv[]) {



    std::cout << "Running on: " << HostSpace::name() << "\n";
    {
        const int N = 1e7;
        const int num_futures = 50;
        Kokkos::Timer timer;

        std::vector<hpx::future<void>> futures;
        for (int i = 0; i < num_futures; i++) {
            Kokkos::View<double*, HostSpace> private_data("private_data_" + std::to_string(i), N);

            switch (i % 3)
            {
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
        std::cout << "All kernels running in parallel took " << time << " seconds." << std::endl;
    }
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