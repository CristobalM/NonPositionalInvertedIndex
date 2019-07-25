cmake_minimum_required(VERSION 3.14)
project(mifluz_comparison)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "-O3 -Wall -Wextra")

include_directories(../../include)
link_directories(../../build)


add_executable(mifluz_comparison mifluz_comparison.cpp)
target_link_libraries(mifluz_comparison NonPosInvIdx)

