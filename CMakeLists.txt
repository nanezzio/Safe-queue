cmake_minimum_required(VERSION 3.10)
project(ThreadPool)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

add_executable(thread_pool main.cpp thread_pool.cpp)

target_include_directories(thread_pool PRIVATE include)