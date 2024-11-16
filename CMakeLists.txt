cmake_minimum_required(VERSION 3.28)
project(VideoGamesOptimizationTechniques)

set(CMAKE_CXX_STANDARD 23)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

if (UNIX)
    set(CMAKE_LINKER_TYPE MOLD)
endif()

if (MSVC)
    add_compile_options(/arch:SSE4.1)
else ()
    add_compile_options(-msse4.1)
endif ()

function(target_link_stdlib)
    if (WIN32)
        target_link_libraries(${ARGV0} -static-libstdc++)
    else()
        target_link_libraries(${ARGV0} stdc++)
    endif()
endfunction()

add_subdirectory(thirdparty)

add_subdirectory(CommonHeaders)

add_subdirectory(CacheExperiments)
add_subdirectory(SieveOfEratosthenes)
add_subdirectory(BitwiseOperations)
add_subdirectory(ContainersComparison)
add_subdirectory(KnapsackProblem)
add_subdirectory(DotProduct)
