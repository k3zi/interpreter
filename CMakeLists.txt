# Set minimum version required.
cmake_minimum_required(VERSION 2.8.0)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Require C++11.
if(${CMAKE_MAJOR_VERSION} GREATER 2)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
  set(CMAKE_CXX_STANDARD 11)
  set(CMAKE_CXX_EXTENSIONS OFF)
else()
  # This should work on 2.8.12
  add_compile_options(-std=c++11)
endif()

# Name the project.
project(Parser)

# Include header files from the "include" directory.
include_directories(include)

# Include sources from the "lib" directory.
file(GLOB LIBRARY_SOURCES "lib/core/**/*.cpp")

# Generate the Tokenizer.
add_executable(Tokenizer "tools/core/Tokenizer.cpp" ${LIBRARY_SOURCES})

# Generate the Parser.
add_executable(Parser "tools/core/Parser.cpp" ${LIBRARY_SOURCES})

# Generate the Interpreter.
add_executable(Interpreter "tools/core/Interpreter.cpp" ${LIBRARY_SOURCES})

# Generate the testing suite.
file(GLOB TEST_SOURCES "test/*.cpp")
add_executable(Tester ${TEST_SOURCES} ${LIBRARY_SOURCES})
