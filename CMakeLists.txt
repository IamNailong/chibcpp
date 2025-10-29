cmake_minimum_required(VERSION 3.22)
project(chibcpp VERSION 0.1
        LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_RUNTIME_OUTPUT_LIB_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

add_executable(chibcpp
    main.cpp
)