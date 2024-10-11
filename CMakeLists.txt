cmake_minimum_required(VERSION 3.28)
project(VideoGamesOptimizationTechniques)

set(CMAKE_CXX_STANDARD 23)

set(CMAKE_LINKER_TYPE MOLD)

add_subdirectory(CacheExperiments)
add_subdirectory(SieveOfEratosthenes)
