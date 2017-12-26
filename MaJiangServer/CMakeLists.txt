cmake_minimum_required(VERSION 3.9)
project(MaJiangServer)

set(CMAKE_CXX_STANDARD 11)


add_executable(MaJiangServer main.cpp MaJiangType.h MaJiangCommand.h)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(MaJiangServer Threads::Threads)