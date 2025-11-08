cmake_minimum_required(VERSION 3.16)
project(chibcpp VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(include)

# Source files
set(SOURCES
    src/AST.cpp
    src/CommandLine.cpp
    src/Diagnostic.cpp
    src/TokenKinds.cpp
    src/Tokenizer.cpp
    src/Parser.cpp
    src/CodeGenerator.cpp
    main.cpp
)

# Create executable
add_executable(chibcpp ${SOURCES})

# Set output directory
set_target_properties(chibcpp PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)
