cmake_minimum_required(VERSION 3.28)
project(CommonHeaders)

file(GLOB_RECURSE SOURCE_FILES "*.h" "*.cpp")

add_library(${PROJECT_NAME} ${SOURCE_FILES})

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/Public)
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/Private)

message(PROJECT_SOURCE_DIR="${PROJECT_SOURCE_DIR}")

target_link_stdlib(${PROJECT_NAME})