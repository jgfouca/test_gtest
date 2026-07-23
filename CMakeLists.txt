cmake_minimum_required(VERSION 3.14)
project(GoogleTestRandomDemo CXX)

# Force independent build standard
# Kokkos requires C++20 or newer starting with Kokkos 4.0+
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(GTEST_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/gtest)

# Enable OpenMP or Serial backend logic here if needed
set(Kokkos_ENABLE_SERIAL ON CACHE BOOL "" FORCE)
# set(Kokkos_ENABLE_OPENMP ON CACHE BOOL "" FORCE) # Uncomment if you want OpenMP

# Add the local source directory directly into CMake
add_subdirectory(external/kokkos)

# ---------------------------------------------------------
# 2. Configure and Build Kokkos Kernels
# ---------------------------------------------------------
# Tell Kokkos Kernels which basic mathematical layouts to optimize
set(KokkosKernels_ENABLE_COMPONENT_SPARSE ON CACHE BOOL "" FORCE)

add_subdirectory(external/kokkos-kernels)

add_library(
  kokkoskernels_gtest
  ${GTEST_SOURCE_DIR}/gtest/gtest-all.cc)

# Disables pthreads, this is a problem for serial builds in Trilinos & Sierra if it's enabled.
target_compile_definitions(kokkoskernels_gtest PUBLIC "-DGTEST_HAS_PTHREAD=0")
target_include_directories(kokkoskernels_gtest PUBLIC $<BUILD_INTERFACE:${GTEST_SOURCE_DIR}>)

#Gtest minimally requires C++11
target_compile_features(kokkoskernels_gtest PUBLIC cxx_std_11)

# 2. Declare your test executable
add_executable(run_my_tests
    main.cpp
    my_tests.cpp
)

# 3. Link GoogleTest binaries and threading libraries
target_link_libraries(run_my_tests
  kokkoskernels_gtest
  Kokkos::kokkos         # Links Core Framework
  Kokkos::kokkoskernels  # Links Mathematical Kernels
)

# 4. Enable modern CMake test tracking discovery
include(GoogleTest)
gtest_discover_tests(run_my_tests)
