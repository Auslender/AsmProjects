project(trampoline)
cmake_minimum_required(VERSION 3.8)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS -g)
add_executable(trampoline bytecode.h argument_classes.h instruction.h trampoline.h main.cpp)