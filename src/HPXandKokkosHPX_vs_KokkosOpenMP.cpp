#include <hpx/init.hpp>
#include <hpx/future.hpp>
#include <cstdint>
#include <Kokkos_Core.hpp>
#include <iostream>
#include <cmath>

using HostSpace = Kokkos::DefaultHostExecutionSpace;

void fill_kernel(Kokkos::View<double*, HostSpace> data, int N) {
    Kokkos::parallel_for("fill_data", Kokkos::RangePolicy<HostSpace>(0, N), KOKKOS_LAMBDA(const int i) {
        double temp_val = data(i);
        for (int k = 0; k < 1000; ++k) {
            temp_val = std::sin(std::sqrt(temp_val + 1.0));
        }
        data(i) = 1.0;
    });
    Kokkos::fence();
    std::cout << "Fill kernel complete." << std::endl;
}

void reduce_kernel(Kokkos::View<double*, HostSpace> data, int N) {
    double sum = 0.0;
    Kokkos::Timer timer;
    Kokkos::parallel_reduce("sum_reduction", Kokkos::RangePolicy<HostSpace>(0, N), KOKKOS_LAMBDA(const int i, double& lsum) {
        double temp_val = data(i);
        for (int k = 0; k < 1000; ++k) {
            temp_val = std::sin(std::sqrt(temp_val + 1.0));
        }
        lsum += temp_val;
    }, sum);
    std::cout << "Reduce kernel complete." << std::endl;
}

void scatter_kernel(Kokkos::View<double*, HostSpace> data, int N) {
    Kokkos::parallel_for("scatter_op", Kokkos::RangePolicy<HostSpace>(0, N), KOKKOS_LAMBDA(const int i) {
      double temp_val = static_cast<double>(i) / N;
        for (int k = 0; k < 1000; ++k) {
            temp_val = std::cos(std::sqrt(temp_val + 1.0));
        }
        data(i) = temp_val;
    });
    std::cout << "Scatter kernel complete." << std::endl;
}

int hpx_main(int argc, char* argv[]) {
    Kokkos::initialize(argc, argv);
    std::cout << "Running on: " << HostSpace::name() << "\n";
    {
        const int N = 1e7;
        Kokkos::View<double*, HostSpace> data("data", N);
        Kokkos::Timer timer;
        hpx::future<void> f1 = hpx::async(hpx::bind(fill_kernel, data, N));
        hpx::future<void> f2 = hpx::async(hpx::bind(reduce_kernel, data, N));
        hpx::future<void> f3 = hpx::async(hpx::bind(scatter_kernel, data, N));

        hpx::wait_all(f1, f2, f3);

        Kokkos::fence();
        double time = timer.seconds();
        std::cout << "Kernels took " << time << " seconds." << std::endl;

    }
    Kokkos::finalize();
    return hpx::local::finalize();
}

int main(int argc, char* argv[]) {
    return hpx::local::init(hpx_main, argc, argv);
}