# Set minimum version required
cmake_minimum_required(VERSION 3.11.4)

# Name the project
project(Tokenizer)

# Include header files from the "include" directory
include_directories(include)

# Include sources from the "lib" directory
file(GLOB SOURCES "lib/*.cpp")

# Generate an executable named "Tokenizer"
add_executable(Tokenizer "${CMAKE_CURRENT_SOURCE_DIR}/Tokenizer.cpp" ${SOURCES})
