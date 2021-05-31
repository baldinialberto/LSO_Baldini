cmake_minimum_required(VERSION 3.19)
project(LSO_LIB C)

set(CMAKE_C_STANDARD 99)

add_library(LSO_LIB SHARED
        mem_utils.c mem_utils.h
        file_utils.c file_utils.h
        hash_utils.c hash_utils.h
        list_utils.c list_utils.h string_utils.c string_utils.h error_utils.h thread_utils.c thread_utils.h array_utils.c array_utils.h)