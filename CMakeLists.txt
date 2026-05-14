cmake_minimum_required(VERSION 3.10)

project(microvm VERSION 0.1 LANGUAGES C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_executable(microvm src/microvm.c)
