#include <iostream>
#include <Kokkos_Core.hpp>

using ExecSpace = Kokkos::DefaultHostExecutionSpace;
using View = Kokkos::View<double*, typename ExecSpace::memory_space>;

int main(int argc, char* argv[]) {
    Kokkos::initialize(argc, argv);
    std::cout << ExecSpace::name() << "\n";
    {
        constexpr int N = 10'000'000;
        constexpr int work_factor = 10000 / 2; // Increased work per iteration

        View a("a", N);

        Kokkos::parallel_for(
            "Init", Kokkos::RangePolicy<ExecSpace>(0, N),
            KOKKOS_LAMBDA(const int i) {
                a(i) = static_cast<double>(i);
                // Time-wasting change: a simple loop to add extra work
                for (int j = 0; j < work_factor; ++j) {
                    a(i) = a(i) * 1.0000000001;
                }
            });
        Kokkos::fence();

        double sum = 0.0;
        Kokkos::parallel_reduce(
            "Reduce", Kokkos::RangePolicy<ExecSpace>(0, N),
            KOKKOS_LAMBDA(const int i, double& lsum) {
                lsum += a(i);
            }, sum);
        Kokkos::fence();

        const double expected = 0.5 * (N - 1) * N;
        std::cout << "Sum: " << sum << "\nExpected: " << expected << "\n"
                  << ((sum == expected) ? "OK\n" : "Mismatch\n");
    }
    Kokkos::finalize();  

    return 0;
}