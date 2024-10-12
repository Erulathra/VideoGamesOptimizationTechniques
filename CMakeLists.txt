cmake_minimum_required(VERSION 3.28)
project(VideoGamesOptimizationTechniques)

set(CMAKE_CXX_STANDARD 23)

if (!WIN32)
    set(CMAKE_LINKER_TYPE MOLD)
endif()

add_subdirectory(CacheExperiments)
add_subdirectory(SieveOfEratosthenes)
add_subdirectory(BitwiseOperations)
