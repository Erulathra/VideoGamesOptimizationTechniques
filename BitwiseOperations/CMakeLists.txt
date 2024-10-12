cmake_minimum_required(VERSION 3.28)
project(BitwiseOperations)

add_executable(${PROJECT_NAME} main.cpp)

if (WIN32)
    target_link_libraries(${PROJECT_NAME} -static-libstdc++)
else()
    target_link_libraries(${PROJECT_NAME} stdc++)
endif()
