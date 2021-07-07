cmake_minimum_required(VERSION 3.19)
project(LSO_Baldini C)

set(CMAKE_C_STANDARD 11)

include_directories(./include ./libs/LSO_LIB)

add_executable(LSO_Baldini
        src/server.c include/server.h
        src/client.c include/client.h
        src/tester.c include/server_message.h
        src/serverapi.c include/serverapi.h
        src/dir_utils.c include/dir_utils.h
        libs/LSO_LIB/array_utils.c
        libs/LSO_LIB/common_utils.h libs/LSO_LIB/error_utils.h
        libs/LSO_LIB/file_utils.h libs/LSO_LIB/file_utils.c
        libs/LSO_LIB/hash_utils.h libs/LSO_LIB/hash_utils.c
        libs/LSO_LIB/list_utils.h libs/LSO_LIB/list_utils.c
        libs/LSO_LIB/mem_utils.h libs/LSO_LIB/mem_utils.c
        libs/LSO_LIB/string_utils.h libs/LSO_LIB/string_utils.c
        libs/LSO_LIB/thread_utils.h libs/LSO_LIB/thread_utils.c
        src/poll_utils.c include/poll_utils.h)