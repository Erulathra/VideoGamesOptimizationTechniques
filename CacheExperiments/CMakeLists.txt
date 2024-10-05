cmake_minimum_required(VERSION 3.28)
project(CacheExperiments)

set(CMAKE_CXX_STANDARD 20)

add_executable(${PROJECT_NAME} cache_perf.cpp)

if (WIN32)
    target_link_libraries(${PROJECT_NAME} -static-libstdc++)
else()
    target_link_libraries(${PROJECT_NAME} stdc++)
endif()
