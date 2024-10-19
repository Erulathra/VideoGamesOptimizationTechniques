cmake_minimum_required(VERSION 3.28)
project(SieveOfEratosthenes)

add_executable(${PROJECT_NAME} main.cpp)

target_link_stdlib(${PROJECT_NAME})
