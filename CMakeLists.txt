cmake_minimum_required(VERSION 3.25)

set(PROJECT modulo)

project(${PROJECT} LANGUAGES CXX)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(SOURCES 
    src/mod.cpp
    src/mod_simd_shr.cpp
    src/mod_simd_sub.cpp
    src/mod_simd_div.cpp
    ../highway/hwy/print.cc
    ../highway/hwy/targets.cc
    ../highway/hwy/per_target.cc
    ../highway/hwy/abort.cc
    # ../traktor/src/calculate.cpp
)

add_executable(${PROJECT} ${SOURCES})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

target_compile_features(${PROJECT} PRIVATE cxx_std_17)
target_compile_definitions(${PROJECT} PUBLIC "$<$<CONFIG:DEBUG>:DEBUG>")

target_include_directories(${PROJECT} PRIVATE "../highway")

#
# GOOGLE BENCHMARK
#
add_subdirectory(benchmark)
target_link_libraries(${PROJECT}  benchmark::benchmark)



#target_compile_options(${PROJECT} PRIVATE -Wall -Wextra -Wpedantic -masm=intel -save-temps=obj -march=native)
target_compile_options(${PROJECT} PRIVATE -march=native -mtune=native)
# -march=native
# -march=znver2
# -mavx2

message(STATUS "Compiler: ${CMAKE_CXX_COMPILER} ${CMAKE_CXX_COMPILER_VERSION}")