cmake_minimum_required(VERSION 3.28)
project(CacheExperiments)

add_executable(${PROJECT_NAME} cache_perf.cpp)

target_link_stdlib(${PROJECT_NAME})
