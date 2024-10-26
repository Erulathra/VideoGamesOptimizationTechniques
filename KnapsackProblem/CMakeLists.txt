cmake_minimum_required(VERSION 3.28)
project(KnapsackProblem)

add_executable(${PROJECT_NAME} main.cpp)

target_link_stdlib(${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME} CommonHeaders)
target_link_libraries(${PROJECT_NAME} lazycsv)
target_link_libraries(${PROJECT_NAME} cxxopts)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E create_symlink
        ${CMAKE_SOURCE_DIR}/res
        ${CMAKE_CURRENT_BINARY_DIR}/res)
