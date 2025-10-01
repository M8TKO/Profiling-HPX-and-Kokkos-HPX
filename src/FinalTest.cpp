#include <hpx/init.hpp>
#include <hpx/future.hpp>
#include <hpx/hpx.hpp>
#include <cstdint>
#include <Kokkos_Core.hpp>
#include <iostream>
#include <cmath>
#include <fstream>
#include <vector>
#include <cstdint>

const int N = 1e5;
int indexName = 0;


using HostExecSpace = Kokkos::DefaultHostExecutionSpace;
using DeviceExecSpace = Kokkos::DefaultExecutionSpace;

using HostView = Kokkos::View<double*, HostExecSpace>;
using DeviceView = Kokkos::View<double*, DeviceExecSpace>;

using DeviceMirrorView = typename DeviceView::HostMirror;

DeviceMirrorView fillHost( DeviceMirrorView data){
    Kokkos::parallel_for("fill_host", Kokkos::RangePolicy<HostExecSpace>(0, N), KOKKOS_LAMBDA(const int i) {
        data(i) = 1.0;
    });
    HostExecSpace().fence();
    return data;
}

DeviceView fillDevice(DeviceExecSpace exec_space, DeviceView data){
    
    Kokkos::parallel_for("fill_device" +std::to_string(indexName++), Kokkos::RangePolicy<DeviceExecSpace>( exec_space, 0, N), KOKKOS_LAMBDA(const int i) {
        data(i) = 3.0;
    });
    exec_space.fence();
    return data;
}

DeviceMirrorView scalarHost( DeviceMirrorView data, double alpha){
    Kokkos::parallel_for("scalar", Kokkos::RangePolicy<HostExecSpace>(0, N), KOKKOS_LAMBDA(const int i) {
        data(i) *= alpha;
    });
    HostExecSpace().fence();
    return data;
}

DeviceView addDevice(DeviceExecSpace exec_space, DeviceView a, DeviceView b, DeviceView c){
    Kokkos::parallel_for("add"+std::to_string(indexName++), Kokkos::RangePolicy<DeviceExecSpace>( exec_space, 0, N), KOKKOS_LAMBDA(const int i) {
        c(i) = a(i) + b(i);
    });
    exec_space.fence();
    return c;
}


hpx::future<DeviceMirrorView> z_future;
hpx::future<double> alpha_future;
hpx::future<DeviceView> u_future;
hpx::future<DeviceView> w_future;
hpx::future<DeviceView> t_future;
hpx::future<DeviceView> result_future;

hpx::future<DeviceView> z_device_scaled_future;
hpx::future<DeviceMirrorView> z_;
hpx::future<void> copy_future;

void tasking(auto partitionSpaces, DeviceView &u, DeviceMirrorView &z, DeviceView &w, DeviceView &t, DeviceView &z_device_scaled, DeviceView &result_device) {
    double alpha = 2.0;
    hpx::promise<DeviceView> p;

    z_future = hpx::make_ready_future(z);
    alpha_future = hpx::make_ready_future(alpha);
    u_future = hpx::make_ready_future(u);
    w_future = hpx::make_ready_future(w);
    t_future = hpx::make_ready_future(t);
    result_future = hpx::make_ready_future(result_device);
    z_device_scaled_future = p.get_future();

    hpx::future<DeviceMirrorView> filled_z_future = z_future.then(
        [](hpx::future<DeviceMirrorView> z_fut) {
            auto result = fillHost(z_fut.get());
            return result;
        }
    );

    z_ = hpx::when_all(filled_z_future, alpha_future).then(
        [](auto&& f) {
            auto&& [z_fut, alpha_fut] = f.get();
            return scalarHost(z_fut.get(), alpha_fut.get());
        }
    );

    z_.then(
        [z_device_scaled, p = std::move(p)](hpx::future<DeviceMirrorView> scaled_z_fut) mutable {
            Kokkos::deep_copy(z_device_scaled, scaled_z_fut.get());
            p.set_value(DeviceView(z_device_scaled));
        }
    );

    hpx::future<DeviceView> filled_u_future = u_future.then(
        [&partitionSpaces](hpx::future<DeviceView> u_fut) {
            return fillDevice(partitionSpaces[0], u_fut.get());
        }
    );

    hpx::future<DeviceView> filled_w_future = w_future.then(
        [&partitionSpaces](hpx::future<DeviceView> w_fut) {
            return fillDevice(partitionSpaces[1], w_fut.get());
        }
    );

    t_future = hpx::when_all(filled_u_future, filled_w_future, t_future).then(
        [&partitionSpaces](auto&& f) {
            auto&& [u_fut, w_fut, t_fut] = f.get();
            return addDevice(partitionSpaces[2], u_fut.get(), w_fut.get(), t_fut.get());
        }
    );

    result_future = hpx::when_all(t_future, z_device_scaled_future, result_future).then(
        [&partitionSpaces](auto&& f) {
            auto&& [t_fut, z_device_fut, result_fut] = f.get();
            return addDevice(partitionSpaces[3], t_fut.get(), z_device_fut.get(), result_fut.get());
        }
    );

    copy_future = result_future.then(
        [&z](hpx::future<DeviceView> res_fut) {
            Kokkos::deep_copy(z, res_fut.get());
        }
    );

    copy_future.wait();
}

void totalKernel(DeviceView &z_totalKernel, DeviceView &u_totalKernel, DeviceView &w_totalKernel, DeviceView &out_totalKernel, DeviceMirrorView &z_totalKernel_mirror){
    
    double a = 1.0;
    double b = 3.0;
    double c = 3.0;
    double alpha = 2.0;

    Kokkos::parallel_for("total kernel", Kokkos::RangePolicy<DeviceExecSpace>(0, N), KOKKOS_LAMBDA(int i) {
        z_totalKernel(i) = a; 
        u_totalKernel(i) = b;
        w_totalKernel(i) = c;
        out_totalKernel(i) = alpha * z_totalKernel(i) + u_totalKernel(i) + w_totalKernel(i);
    });
    Kokkos::fence();
    Kokkos::deep_copy(z_totalKernel_mirror, out_totalKernel);

}

void serialKernels(DeviceView &z_serialKernels, DeviceView &u_serialKernels, DeviceView &w_serialKernels, DeviceView &t_serialKernels, DeviceView &y_serialKernels, DeviceView &out_serialKernels){

    double a = 1.0;
    double b = 3.0;
    double c = 3.0;
    double alpha = 2.0;

    Kokkos::parallel_for("fill z", Kokkos::RangePolicy<DeviceExecSpace>(0, N), KOKKOS_LAMBDA (int i) {
        z_serialKernels(i)  = a;
    });
    Kokkos::fence();

    Kokkos::parallel_for("fill u", Kokkos::RangePolicy<DeviceExecSpace>(0, N), KOKKOS_LAMBDA (int i) {
        u_serialKernels(i)  = b;
    });
    Kokkos::fence();

    Kokkos::parallel_for("fill w", Kokkos::RangePolicy<DeviceExecSpace>(0, N), KOKKOS_LAMBDA (int i) {
        w_serialKernels(i)  = c;
    });
    Kokkos::fence();

    Kokkos::parallel_for("t=u+w", Kokkos::RangePolicy<DeviceExecSpace>(0, N), KOKKOS_LAMBDA (int i) {
        t_serialKernels(i) = u_serialKernels(i) + w_serialKernels(i);
    });
    Kokkos::fence();

    Kokkos::parallel_for("y = alpha*z", Kokkos::RangePolicy<DeviceExecSpace>(0, N), KOKKOS_LAMBDA (int i) {
            y_serialKernels(i) = alpha*z_serialKernels(i);
    });
    Kokkos::fence();

    Kokkos::parallel_for("out=y+t", Kokkos::RangePolicy<DeviceExecSpace>(0, N), KOKKOS_LAMBDA (int i) {
        out_serialKernels(i) = y_serialKernels(i) + t_serialKernels(i);
    });
    Kokkos::fence();
    DeviceMirrorView z_serialKernels_mirror = Kokkos::create_mirror_view(out_serialKernels);
    Kokkos::deep_copy(z_serialKernels_mirror, out_serialKernels);
}

int fun(int x){ 
    std::this_thread::sleep_for(std::chrono::milliseconds(100));    
    return x+1;
}

int hpx_main(int argc, char* argv[]) {
    
    std::cout << "Host execution space: " << HostExecSpace::name() << "\n";
    std::cout << "Device execution space: " << DeviceExecSpace::name() << "\n\n";

    int Ns = 100;
    int nwarm = 10;


    {
        DeviceView u("u", N);
        DeviceMirrorView z = Kokkos::create_mirror_view(u);
        DeviceView w("w", N);
        DeviceView t("t", N);
        DeviceView z_device_scaled = Kokkos::create_mirror_view( DeviceExecSpace(), z);
        DeviceView result_device = Kokkos::create_mirror_view( DeviceExecSpace(), z);    
        auto partitionSpaces = Kokkos::Experimental::partition_space(DeviceExecSpace(), std::vector<int>( 4, 1));

        std::vector<double> times(Ns,0.0);
        for(int j = 0; j < nwarm; j++)
            tasking( partitionSpaces, u, z, w, t, z_device_scaled, result_device);


        Kokkos::Timer timer;
        for(int j = 0; j < Ns; j++){
            timer.reset();
            tasking( partitionSpaces, u, z, w, t, z_device_scaled, result_device);
            times[j] += timer.seconds();
        }
         
        double min_time = *std::min_element(times.begin(), times.end());
        std::cout << "Tasking: " << std::endl;
        std::cout << "Maximum Effective Bandwidth: " << 4 * sizeof(double) * N / 1e9/ min_time << " GB/sec." << std::endl;
    }

    // {
    //     DeviceView z_totalKernel("z", N);
    //     DeviceView u_totalKernel("u", N);
    //     DeviceView w_totalKernel("w", N);
    //     DeviceView out_totalKernel("out", N);
    //     DeviceMirrorView z_totalKernel_mirror = Kokkos::create_mirror_view(out_totalKernel);
    //     std::vector<double> times(Ns,0.0);
    //     for(int j = 0; j < nwarm; j++)
    //         totalKernel(z_totalKernel, u_totalKernel, w_totalKernel, out_totalKernel, z_totalKernel_mirror);

    //     Kokkos::Timer timer;
    //     for(int j = 0; j < Ns; j++){
    //         timer.reset();
    //         totalKernel(z_totalKernel, u_totalKernel, w_totalKernel, out_totalKernel, z_totalKernel_mirror);
    //         times[j] += timer.seconds();
    //     }
         
    //     double min_time = *std::min_element(times.begin(), times.end());
    //     std::cout << std::endl << "Total kernel: " << std::endl;
    //     std::cout << "Maximum Effective Bandwidth: " << 4 * sizeof(double) * N / 1e9/ min_time << " GB/sec." << std::endl;
    // }

    // {
    //     DeviceView z_serialKernels("z", N);
    //     DeviceView u_serialKernels("u", N);
    //     DeviceView w_serialKernels("w", N); 
    //     DeviceView t_serialKernels("t", N);
    //     DeviceView y_serialKernels("y", N);
    //     DeviceView out_serialKernels("out", N);

    //     std::vector<double> times(Ns,0.0);
    //     for(int j = 0; j < nwarm; j++)
    //         serialKernels(z_serialKernels, u_serialKernels, w_serialKernels, t_serialKernels, y_serialKernels, out_serialKernels);

    //     Kokkos::Timer timer;
    //     for(int j = 0; j < Ns; j++){
    //         timer.reset();
    //         serialKernels(z_serialKernels, u_serialKernels, w_serialKernels, t_serialKernels, y_serialKernels, out_serialKernels);
    //         times[j] += timer.seconds();
    //     }
         
    //     double min_time = *std::min_element(times.begin(), times.end());
    //     std::cout << std::endl << "Serial kernels: " << std::endl;
    //     std::cout << "Maximum Effective Bandwidth: " << 4 * sizeof(double) * N / 1e9/ min_time << " GB/sec." << std::endl;
    // }

    Kokkos::finalize();
    if( HostExecSpace::name() != "HPX" )
        return hpx::local::finalize();
    else
        return 0;
}

int main(int argc, char* argv[]) {
    Kokkos::initialize(argc, argv);


   if( HostExecSpace::name() != "HPX" )
        return hpx::local::init(hpx_main, argc, argv);
    else
        return hpx_main(argc, argv);
}