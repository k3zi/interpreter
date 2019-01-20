# Set minimum version required
cmake_minimum_required(VERSION 2.8.0)

# Require C++11
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

# Name the project
project(Tokenizer)

# Include header files from the "include" directory
include_directories(include)

# Include sources from the "lib" directory
file(GLOB SOURCES "lib/*.cpp")

# Generate an executable named "Tokenizer"
add_executable(Tokenizer TokenizerMain.cpp ${SOURCES})
