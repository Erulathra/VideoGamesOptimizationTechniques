cmake_minimum_required(VERSION 3.28)
project(GaussianElimination)

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} CommonHeaders)

target_link_stdlib(${PROJECT_NAME})
