cmake_minimum_required(VERSION 3.21)
project(HPX_KOKKOS_TASKING VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)
set(FETCHCONTENT_UPDATES_DISCONNECTED ON)
FetchContent_Declare(
  Kokkos
  GIT_REPOSITORY https://github.com/kokkos/kokkos.git
  GIT_TAG release-candidate-4.7.01
  GIT_SHALLOW TRUE
)

FetchContent_Declare(
  Kokkos-tools
  GIT_REPOSITORY https://github.com/kokkos/kokkos-tools.git
  GIT_TAG 1c843f3b38f8f4aa8c3f865d54744151d523f724
  GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(Kokkos Kokkos-tools)

find_package(HPX REQUIRED CONFIG)

add_executable(exec src/KokkosHPXBackend.cpp)
target_compile_features(exec PRIVATE cxx_std_20)
target_link_libraries(exec
  PRIVATE
    Kokkos::kokkos
    HPX::hpx
    HPX::wrap_main
)

add_executable(exec_hpx src/HPX.cpp)
target_compile_features(exec_hpx PRIVATE cxx_std_20)
target_link_libraries(exec_hpx
  PRIVATE
    HPX::hpx
    HPX::wrap_main
)

