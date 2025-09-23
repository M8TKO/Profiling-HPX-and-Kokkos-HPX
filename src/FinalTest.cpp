#include <hpx/init.hpp>
#include <hpx/future.hpp>
#include <cstdint>
#include <Kokkos_Core.hpp>
#include <iostream>
#include <cmath>
#include <fstream>
#include <vector>
#include <cstdint>

const int N = 1e4;
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

int hpx_main(int argc, char* argv[]) {
    
    std::cout << "Host execution space: " << HostExecSpace::name() << "\n";
    std::cout << "Device execution space: " << DeviceExecSpace::name() << "\n\n";

    
    {
        DeviceView u("u_device", N);
        DeviceMirrorView z = Kokkos::create_mirror_view(u);;
        DeviceView w("w_device", N);
        DeviceView t("t_device", N);
        DeviceView z_device_scaled = Kokkos::create_mirror_view( DeviceExecSpace(), z);
        DeviceView result_device = Kokkos::create_mirror_view( DeviceExecSpace(), z);
        double alpha = 2.0;

        auto partitionSpaces = Kokkos::Experimental::partition_space(DeviceExecSpace(), std::vector<int>( 4, 1));

        hpx::future<DeviceMirrorView> z_future = hpx::make_ready_future(z);
        hpx::future<double> alpha_future = hpx::make_ready_future(alpha);
        hpx::future<DeviceView> u_future = hpx::make_ready_future(u);
        hpx::future<DeviceView> w_future = hpx::make_ready_future(w);
        hpx::future<DeviceView> t_future = hpx::make_ready_future(t);
        hpx::future<DeviceView> result_future = hpx::make_ready_future(result_device);
        hpx::promise<DeviceView> p;
        hpx::future<DeviceView> z_device_scaled_future = p.get_future();;
        

        z_future = hpx::dataflow( hpx::unwrapping(fillHost), z_future);
        hpx::future<DeviceMirrorView> z_ = hpx::dataflow( hpx::unwrapping(scalarHost), z_future, alpha_future);

        z_.then([z_device_scaled, &z_device_scaled_future, &p](hpx::future<DeviceMirrorView> scaled_z_future){
            Kokkos::deep_copy(z_device_scaled, scaled_z_future.get());
            p.set_value(DeviceView(z_device_scaled));
        });

        u_future = hpx::dataflow( hpx::unwrapping(fillDevice), partitionSpaces[0], u_future);
        w_future = hpx::dataflow( hpx::unwrapping(fillDevice), partitionSpaces[1], w_future);

        t_future = hpx::dataflow( hpx::unwrapping(addDevice), partitionSpaces[2], u_future, w_future, t_future);

        result_future = hpx::dataflow( hpx::unwrapping(addDevice), partitionSpaces[3], t_future, z_device_scaled_future, result_future);

        hpx::future<void> copy_future = result_future.then([&z](hpx::future<DeviceView> result_future){
            Kokkos::deep_copy(z, result_future.get());
        });
        
        copy_future.wait();
        Kokkos::fence();
        std::cout << "z(" << 0 << ") = " << z(0) << std::endl;
        Kokkos::fence();
    }

    Kokkos::finalize();
    return hpx::local::finalize();
}

int main(int argc, char* argv[]) {
    Kokkos::initialize(argc, argv);
    return hpx::local::init(hpx_main, argc, argv);
}