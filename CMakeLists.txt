cmake_minimum_required(VERSION 3.14)
project(NonPosInvIdx)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_FLAGS "-O3 -Wall -Wextra")


# SDSL install location
include_directories(~/include)
link_directories(~/lib)

include_directories(${CMAKE_SOURCE_DIR}/lib)

include_directories(include)

set(INCLUDES_FILES
        TermOrdering.hpp
        TermGrouping.hpp
        InvertedIndex.hpp
        WTHGog.hpp
        BVHGog.hpp
        DocumentInfo.hpp
        DocumentsHandler.hpp
        SafeQueue.hpp
        CircularQueue.hpp
        )

set(SOURCES_FILES
        DocumentInfo.cpp
        DocumentsHandler.cpp)

set(INCLUDES_PATHS "")
set(SOURCES_PATHS "")

foreach (header_file IN LISTS INCLUDES_FILES)
    list(APPEND INCLUDES_PATHS "include/${header_file}")
endforeach ()

foreach (source_file IN LISTS SOURCES_FILES)
    list(APPEND SOURCES_PATHS "src/${source_file}")
endforeach ()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")


add_library(NonPosInvIdx ${INCLUDES_PATHS} ${SOURCES_PATHS})


target_link_libraries(NonPosInvIdx sdsl stdc++fs)

add_executable(example_1 examples/example_1.cpp)
add_executable(example_2_load_from_disk examples/example_2_load_from_disk.cpp)
add_executable(build_inverted_index src/build_inverted_index.cpp)
add_executable(single_query_index src/single_query_index.cpp)
add_executable(multiple_query_index src/multiple_query_index.cpp)
add_executable(prueba_circular src/prueba_circular.cpp)

target_link_libraries(example_1 NonPosInvIdx)
target_link_libraries(example_2_load_from_disk NonPosInvIdx)
target_link_libraries(build_inverted_index NonPosInvIdx)
target_link_libraries(single_query_index NonPosInvIdx)
target_link_libraries(multiple_query_index NonPosInvIdx)
target_link_libraries(prueba_circular NonPosInvIdx)
