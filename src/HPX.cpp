#include <hpx/init.hpp>
#include <hpx/future.hpp>
#include <cstdint>

void hello_worker() {
    std::uint32_t thread_num = hpx::get_worker_thread_num();
    std::cout << "Hello from HPX worker thread " << thread_num << "!\n";
}

int hpx_main() {
    std::uint32_t main_thread_num = hpx::get_worker_thread_num();
    std::cout << "Hello from the main HPX thread " << main_thread_num << ".\n";

    hpx::future<void> f = hpx::async(&hello_worker);
    f.get();

    return hpx::local::finalize();
}

int main(int argc, char* argv[]) {
    return hpx::local::init(hpx_main, argc, argv);
}