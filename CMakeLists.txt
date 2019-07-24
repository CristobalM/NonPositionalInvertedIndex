cmake_minimum_required(VERSION 3.14)
project(NonPosInvIdx)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# SDSL install location
include_directories(~/include)
link_directories(~/lib)


include_directories(include)
set(INCLUDES_FILES
        InvertedIndex.hpp
        WTHGog.hpp
        BVHGog.hpp
        DocumentInfo.h
        DocumentsHandler.h)

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


add_library(NonPosInvIdx ${INCLUDES_PATHS} ${SOURCES_PATHS})


target_link_libraries(NonPosInvIdx sdsl stdc++fs)

add_executable(NonPosInvIdx_example examples/example_1.cpp)
target_link_libraries(NonPosInvIdx_example NonPosInvIdx)

