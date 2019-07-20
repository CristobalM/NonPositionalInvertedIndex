cmake_minimum_required(VERSION 3.14)
project(NonPosInvIdx)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)



include_directories(~/include)
link_directories(~/lib)

add_executable(NonPosInvIdx
        main.cpp
        InvertedIndex.h InvertedIndex.cpp
        WTHGogHandler.h WTHGogHandler.cpp
        DocumentInfo.h DocumentInfo.cpp
        DocumentsHandler.h DocumentsHandler.cpp
        IWordToDocFreqMap.h
        )
target_link_libraries(NonPosInvIdx sdsl stdc++fs)

