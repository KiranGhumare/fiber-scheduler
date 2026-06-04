cmake_minimum_required(VERSION 3.15)
project(fiber_scheduler)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(fiber_scheduler src/main.cpp src/scheduler.cpp src/lockfree_queue.cpp)
target_compile_definitions(fiber_scheduler PRIVATE _XOPEN_SOURCE)
