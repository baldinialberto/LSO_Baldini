cmake_minimum_required(VERSION 3.19)
project(TEST C)

set(CMAKE_C_STANDARD 99)

include_directories(include)

add_executable(TEST src/main.c
        src/sys_utils.c include/sys_utils.h
        src/ptr_utils.c include/ptr_utils.h include/print_utils.h src/server.c include/server.h include/error_utils.h)