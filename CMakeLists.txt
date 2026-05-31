cmake_minimum_required(VERSION 3.14)
project(IteraXive VERSION 1.0.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

find_package(Threads REQUIRED)

add_executable(iteraxive
    src/main.cpp
    src/benchmarkAnalyzer.cpp
)

target_include_directories(iteraxive PRIVATE src)
target_link_libraries(iteraxive PRIVATE Threads::Threads)

option(ITERAXIVE_ENABLE_ASAN "Enable AddressSanitizer" OFF)
if(ITERAXIVE_ENABLE_ASAN)
    target_compile_options(iteraxive PRIVATE -fsanitize=address -fno-omit-frame-pointer)
    target_link_options(iteraxive PRIVATE -fsanitize=address)
endif()

option(ITERAXIVE_ENABLE_UBSAN "Enable UndefinedBehaviorSanitizer" OFF)
if(ITERAXIVE_ENABLE_UBSAN)
    target_compile_options(iteraxive PRIVATE -fsanitize=undefined)
    target_link_options(iteraxive PRIVATE -fsanitize=undefined)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(iteraxive PRIVATE -O3 -DNDEBUG)
elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(iteraxive PRIVATE -O0 -g -Wall -Wextra)
else()
    target_compile_options(iteraxive PRIVATE -O2 -Wall)
endif()

install(TARGETS iteraxive RUNTIME DESTINATION bin)
