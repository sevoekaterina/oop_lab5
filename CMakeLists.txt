cmake_minimum_required(VERSION 3.15)
project(Lab05)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Явно указываем пути для include
include_directories(include)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-fsanitize=address -fno-omit-frame-pointer)
    add_link_options(-fsanitize=address)
endif()

add_executable(lab05
    main.cpp
    src/memory_resource_fixed_list.cpp
)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(lab05 PRIVATE -Wall -Wextra -Wpedantic)
endif()