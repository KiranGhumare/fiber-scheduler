cmake_minimum_required(VERSION 3.15)
project(fiber_scheduler)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# main executable
add_executable(fiber_scheduler src/main.cpp src/scheduler.cpp src/lockfree_queue.cpp)
target_compile_definitions(fiber_scheduler PRIVATE _XOPEN_SOURCE)
target_compile_options(fiber_scheduler PRIVATE -Wno-deprecated-declarations)

# benchmark executable
add_executable(benchmark bench/benchmark.cpp src/scheduler.cpp src/lockfree_queue.cpp)
target_include_directories(benchmark PRIVATE include)
target_compile_definitions(benchmark PRIVATE _XOPEN_SOURCE)
target_compile_options(benchmark PRIVATE -Wno-deprecated-declarations)