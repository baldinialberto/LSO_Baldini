# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.19

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Disable VCS-based implicit rules.
% : %,v


# Disable VCS-based implicit rules.
% : RCS/%


# Disable VCS-based implicit rules.
% : RCS/%,v


# Disable VCS-based implicit rules.
% : SCCS/s.%


# Disable VCS-based implicit rules.
% : s.%


.SUFFIXES: .hpux_make_needs_suffix_list


# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /snap/clion/152/bin/cmake/linux/bin/cmake

# The command to remove a file.
RM = /snap/clion/152/bin/cmake/linux/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/alberto/LSO_Baldini

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/alberto/LSO_Baldini/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/LSO_Baldini.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/LSO_Baldini.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/LSO_Baldini.dir/flags.make

CMakeFiles/LSO_Baldini.dir/src/server.c.o: CMakeFiles/LSO_Baldini.dir/flags.make
CMakeFiles/LSO_Baldini.dir/src/server.c.o: ../src/server.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/alberto/LSO_Baldini/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/LSO_Baldini.dir/src/server.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/LSO_Baldini.dir/src/server.c.o -c /home/alberto/LSO_Baldini/src/server.c

CMakeFiles/LSO_Baldini.dir/src/server.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/LSO_Baldini.dir/src/server.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/alberto/LSO_Baldini/src/server.c > CMakeFiles/LSO_Baldini.dir/src/server.c.i

CMakeFiles/LSO_Baldini.dir/src/server.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/LSO_Baldini.dir/src/server.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/alberto/LSO_Baldini/src/server.c -o CMakeFiles/LSO_Baldini.dir/src/server.c.s

CMakeFiles/LSO_Baldini.dir/src/client.c.o: CMakeFiles/LSO_Baldini.dir/flags.make
CMakeFiles/LSO_Baldini.dir/src/client.c.o: ../src/client.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/alberto/LSO_Baldini/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object CMakeFiles/LSO_Baldini.dir/src/client.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/LSO_Baldini.dir/src/client.c.o -c /home/alberto/LSO_Baldini/src/client.c

CMakeFiles/LSO_Baldini.dir/src/client.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/LSO_Baldini.dir/src/client.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/alberto/LSO_Baldini/src/client.c > CMakeFiles/LSO_Baldini.dir/src/client.c.i

CMakeFiles/LSO_Baldini.dir/src/client.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/LSO_Baldini.dir/src/client.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/alberto/LSO_Baldini/src/client.c -o CMakeFiles/LSO_Baldini.dir/src/client.c.s

CMakeFiles/LSO_Baldini.dir/src/tester.c.o: CMakeFiles/LSO_Baldini.dir/flags.make
CMakeFiles/LSO_Baldini.dir/src/tester.c.o: ../src/tester.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/alberto/LSO_Baldini/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building C object CMakeFiles/LSO_Baldini.dir/src/tester.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/LSO_Baldini.dir/src/tester.c.o -c /home/alberto/LSO_Baldini/src/tester.c

CMakeFiles/LSO_Baldini.dir/src/tester.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/LSO_Baldini.dir/src/tester.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/alberto/LSO_Baldini/src/tester.c > CMakeFiles/LSO_Baldini.dir/src/tester.c.i

CMakeFiles/LSO_Baldini.dir/src/tester.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/LSO_Baldini.dir/src/tester.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/alberto/LSO_Baldini/src/tester.c -o CMakeFiles/LSO_Baldini.dir/src/tester.c.s

CMakeFiles/LSO_Baldini.dir/src/serverapi.c.o: CMakeFiles/LSO_Baldini.dir/flags.make
CMakeFiles/LSO_Baldini.dir/src/serverapi.c.o: ../src/serverapi.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/alberto/LSO_Baldini/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building C object CMakeFiles/LSO_Baldini.dir/src/serverapi.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/LSO_Baldini.dir/src/serverapi.c.o -c /home/alberto/LSO_Baldini/src/serverapi.c

CMakeFiles/LSO_Baldini.dir/src/serverapi.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/LSO_Baldini.dir/src/serverapi.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/alberto/LSO_Baldini/src/serverapi.c > CMakeFiles/LSO_Baldini.dir/src/serverapi.c.i

CMakeFiles/LSO_Baldini.dir/src/serverapi.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/LSO_Baldini.dir/src/serverapi.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/alberto/LSO_Baldini/src/serverapi.c -o CMakeFiles/LSO_Baldini.dir/src/serverapi.c.s

CMakeFiles/LSO_Baldini.dir/src/poll_utils.c.o: CMakeFiles/LSO_Baldini.dir/flags.make
CMakeFiles/LSO_Baldini.dir/src/poll_utils.c.o: ../src/poll_utils.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/alberto/LSO_Baldini/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building C object CMakeFiles/LSO_Baldini.dir/src/poll_utils.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/LSO_Baldini.dir/src/poll_utils.c.o -c /home/alberto/LSO_Baldini/src/poll_utils.c

CMakeFiles/LSO_Baldini.dir/src/poll_utils.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/LSO_Baldini.dir/src/poll_utils.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/alberto/LSO_Baldini/src/poll_utils.c > CMakeFiles/LSO_Baldini.dir/src/poll_utils.c.i

CMakeFiles/LSO_Baldini.dir/src/poll_utils.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/LSO_Baldini.dir/src/poll_utils.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/alberto/LSO_Baldini/src/poll_utils.c -o CMakeFiles/LSO_Baldini.dir/src/poll_utils.c.s

CMakeFiles/LSO_Baldini.dir/src/dir_utils.c.o: CMakeFiles/LSO_Baldini.dir/flags.make
CMakeFiles/LSO_Baldini.dir/src/dir_utils.c.o: ../src/dir_utils.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/alberto/LSO_Baldini/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Building C object CMakeFiles/LSO_Baldini.dir/src/dir_utils.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/LSO_Baldini.dir/src/dir_utils.c.o -c /home/alberto/LSO_Baldini/src/dir_utils.c

CMakeFiles/LSO_Baldini.dir/src/dir_utils.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/LSO_Baldini.dir/src/dir_utils.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/alberto/LSO_Baldini/src/dir_utils.c > CMakeFiles/LSO_Baldini.dir/src/dir_utils.c.i

CMakeFiles/LSO_Baldini.dir/src/dir_utils.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/LSO_Baldini.dir/src/dir_utils.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/alberto/LSO_Baldini/src/dir_utils.c -o CMakeFiles/LSO_Baldini.dir/src/dir_utils.c.s

CMakeFiles/LSO_Baldini.dir/libs/LSO_LIB/array_utils.c.o: CMakeFiles/LSO_Baldini.dir/flags.make
CMakeFiles/LSO_Baldini.dir/libs/LSO_LIB/array_utils.c.o: ../libs/LSO_LIB/array_utils.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/alberto/LSO_Baldini/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Building C object CMakeFiles/LSO_Baldini.dir/libs/LSO_LIB/array_utils.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/LSO_Baldini.dir/libs/LSO_LIB/array_utils.c.o -c /home/alberto/LSO_Baldini/libs/LSO_LIB/array_utils.c

CMakeFiles/LSO_Baldini.dir/libs/LSO_LIB/array_utils.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/LSO_Baldini.dir/libs/LSO_LIB/array_utils.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/alberto/LSO_Baldini/libs/LSO_LIB/array_utils.c > CMakeFiles/LSO_Baldini.dir/libs/LSO_LIB/array_utils.c.i

CMakeFiles/LSO_Baldini.dir/libs/LSO_LIB/array_utils.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/LSO_Baldini.dir/libs/LSO_LIB/array_utils.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/alberto/LSO_Baldini/libs/LSO_LIB/array_utils.c -o CMakeFiles/LSO_Baldini.dir/libs/LSO_LIB/array_utils.c.s

CMakeFiles/LSO_Baldini.dir/libs/LSO_LIB/file_utils.c.o: CMakeFiles/LSO_Baldini.dir/flags.make
CMakeFiles/LSO_Baldini.dir/libs/LSO_LIB/file_utils.c.o: ../libs/LSO_LIB/file_utils.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/alberto/LSO_Baldini/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_8) "Building C object CMakeFiles/LSO_Baldini.dir/libs/LSO_LIB/file_utils.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/LSO_Baldini.dir/libs/LSO_LIB/file_utils.c.o -c /home/alberto/LSO_Baldini/libs/LSO_LIB/file_utils.c

CMakeFiles/LSO_Baldini.dir/libs/LSO_LIB/file_utils.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/LSO_Baldini.dir/libs/LSO_LIB/file_utils.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/alberto/LSO_Baldini/libs/LSO_LIB/file_utils.c > CMakeFiles/LSO_Baldini.dir/libs/LSO_LIB/file_utils.c.i

CMakeFiles/LSO_Baldini.dir/libs/LSO_LIB/file_utils.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/LSO_Baldini.dir/libs/LSO_LIB/file_utils.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/alberto/LSO_Baldini/libs/LSO_LIB/file_utils.c -o CMakeFiles/LSO_Baldini.dir/libs/LSO_LIB/file_utils.c.s

CMakeFiles/LSO_Baldini.dir/libs/LSO_LIB/hash_utils.c.o: CMakeFiles/LSO_Baldini.dir/flags.make
CMakeFiles/LSO_Baldini.dir/libs/LSO_LIB/hash_utils.c.o: ../libs/LSO_LIB/hash_utils.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/alberto/LSO_Baldini/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_9) "Building C object CMakeFiles/LSO_Baldini.dir/libs/LSO_LIB/hash_utils.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/LSO_Baldini.dir/libs/LSO_LIB/hash_utils.c.o -c /home/alberto/LSO_Baldini/libs/LSO_LIB/hash_utils.c

CMakeFiles/LSO_Baldini.dir/libs/LSO_LIB/hash_utils.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/LSO_Baldini.dir/libs/LSO_LIB/hash_utils.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/alberto/LSO_Baldini/libs/LSO_LIB/hash_utils.c > CMakeFiles/LSO_Baldini.dir/libs/LSO_LIB/hash_utils.c.i

CMakeFiles/LSO_Baldini.dir/libs/LSO_LIB/hash_utils.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/LSO_Baldini.dir/libs/LSO_LIB/hash_utils.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/alberto/LSO_Baldini/libs/LSO_LIB/hash_utils.c -o CMakeFiles/LSO_Baldini.dir/libs/LSO_LIB/hash_utils.c.s

CMakeFiles/LSO_Baldini.dir/libs/LSO_LIB/list_utils.c.o: CMakeFiles/LSO_Baldini.dir/flags.make
CMakeFiles/LSO_Baldini.dir/libs/LSO_LIB/list_utils.c.o: ../libs/LSO_LIB/list_utils.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/alberto/LSO_Baldini/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_10) "Building C object CMakeFiles/LSO_Baldini.dir/libs/LSO_LIB/list_utils.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/LSO_Baldini.dir/libs/LSO_LIB/list_utils.c.o -c /home/alberto/LSO_Baldini/libs/LSO_LIB/list_utils.c

CMakeFiles/LSO_Baldini.dir/libs/LSO_LIB/list_utils.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/LSO_Baldini.dir/libs/LSO_LIB/list_utils.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/alberto/LSO_Baldini/libs/LSO_LIB/list_utils.c > CMakeFiles/LSO_Baldini.dir/libs/LSO_LIB/list_utils.c.i

CMakeFiles/LSO_Baldini.dir/libs/LSO_LIB/list_utils.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/LSO_Baldini.dir/libs/LSO_LIB/list_utils.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/alberto/LSO_Baldini/libs/LSO_LIB/list_utils.c -o CMakeFiles/LSO_Baldini.dir/libs/LSO_LIB/list_utils.c.s

CMakeFiles/LSO_Baldini.dir/libs/LSO_LIB/mem_utils.c.o: CMakeFiles/LSO_Baldini.dir/flags.make
CMakeFiles/LSO_Baldini.dir/libs/LSO_LIB/mem_utils.c.o: ../libs/LSO_LIB/mem_utils.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/alberto/LSO_Baldini/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_11) "Building C object CMakeFiles/LSO_Baldini.dir/libs/LSO_LIB/mem_utils.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/LSO_Baldini.dir/libs/LSO_LIB/mem_utils.c.o -c /home/alberto/LSO_Baldini/libs/LSO_LIB/mem_utils.c

CMakeFiles/LSO_Baldini.dir/libs/LSO_LIB/mem_utils.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/LSO_Baldini.dir/libs/LSO_LIB/mem_utils.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/alberto/LSO_Baldini/libs/LSO_LIB/mem_utils.c > CMakeFiles/LSO_Baldini.dir/libs/LSO_LIB/mem_utils.c.i

CMakeFiles/LSO_Baldini.dir/libs/LSO_LIB/mem_utils.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/LSO_Baldini.dir/libs/LSO_LIB/mem_utils.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/alberto/LSO_Baldini/libs/LSO_LIB/mem_utils.c -o CMakeFiles/LSO_Baldini.dir/libs/LSO_LIB/mem_utils.c.s

CMakeFiles/LSO_Baldini.dir/libs/LSO_LIB/string_utils.c.o: CMakeFiles/LSO_Baldini.dir/flags.make
CMakeFiles/LSO_Baldini.dir/libs/LSO_LIB/string_utils.c.o: ../libs/LSO_LIB/string_utils.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/alberto/LSO_Baldini/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_12) "Building C object CMakeFiles/LSO_Baldini.dir/libs/LSO_LIB/string_utils.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/LSO_Baldini.dir/libs/LSO_LIB/string_utils.c.o -c /home/alberto/LSO_Baldini/libs/LSO_LIB/string_utils.c

CMakeFiles/LSO_Baldini.dir/libs/LSO_LIB/string_utils.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/LSO_Baldini.dir/libs/LSO_LIB/string_utils.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/alberto/LSO_Baldini/libs/LSO_LIB/string_utils.c > CMakeFiles/LSO_Baldini.dir/libs/LSO_LIB/string_utils.c.i

CMakeFiles/LSO_Baldini.dir/libs/LSO_LIB/string_utils.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/LSO_Baldini.dir/libs/LSO_LIB/string_utils.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/alberto/LSO_Baldini/libs/LSO_LIB/string_utils.c -o CMakeFiles/LSO_Baldini.dir/libs/LSO_LIB/string_utils.c.s

CMakeFiles/LSO_Baldini.dir/libs/LSO_LIB/thread_utils.c.o: CMakeFiles/LSO_Baldini.dir/flags.make
CMakeFiles/LSO_Baldini.dir/libs/LSO_LIB/thread_utils.c.o: ../libs/LSO_LIB/thread_utils.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/alberto/LSO_Baldini/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_13) "Building C object CMakeFiles/LSO_Baldini.dir/libs/LSO_LIB/thread_utils.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/LSO_Baldini.dir/libs/LSO_LIB/thread_utils.c.o -c /home/alberto/LSO_Baldini/libs/LSO_LIB/thread_utils.c

CMakeFiles/LSO_Baldini.dir/libs/LSO_LIB/thread_utils.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/LSO_Baldini.dir/libs/LSO_LIB/thread_utils.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/alberto/LSO_Baldini/libs/LSO_LIB/thread_utils.c > CMakeFiles/LSO_Baldini.dir/libs/LSO_LIB/thread_utils.c.i

CMakeFiles/LSO_Baldini.dir/libs/LSO_LIB/thread_utils.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/LSO_Baldini.dir/libs/LSO_LIB/thread_utils.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/alberto/LSO_Baldini/libs/LSO_LIB/thread_utils.c -o CMakeFiles/LSO_Baldini.dir/libs/LSO_LIB/thread_utils.c.s

# Object files for target LSO_Baldini
LSO_Baldini_OBJECTS = \
"CMakeFiles/LSO_Baldini.dir/src/server.c.o" \
"CMakeFiles/LSO_Baldini.dir/src/client.c.o" \
"CMakeFiles/LSO_Baldini.dir/src/tester.c.o" \
"CMakeFiles/LSO_Baldini.dir/src/serverapi.c.o" \
"CMakeFiles/LSO_Baldini.dir/src/poll_utils.c.o" \
"CMakeFiles/LSO_Baldini.dir/src/dir_utils.c.o" \
"CMakeFiles/LSO_Baldini.dir/libs/LSO_LIB/array_utils.c.o" \
"CMakeFiles/LSO_Baldini.dir/libs/LSO_LIB/file_utils.c.o" \
"CMakeFiles/LSO_Baldini.dir/libs/LSO_LIB/hash_utils.c.o" \
"CMakeFiles/LSO_Baldini.dir/libs/LSO_LIB/list_utils.c.o" \
"CMakeFiles/LSO_Baldini.dir/libs/LSO_LIB/mem_utils.c.o" \
"CMakeFiles/LSO_Baldini.dir/libs/LSO_LIB/string_utils.c.o" \
"CMakeFiles/LSO_Baldini.dir/libs/LSO_LIB/thread_utils.c.o"

# External object files for target LSO_Baldini
LSO_Baldini_EXTERNAL_OBJECTS =

LSO_Baldini: CMakeFiles/LSO_Baldini.dir/src/server.c.o
LSO_Baldini: CMakeFiles/LSO_Baldini.dir/src/client.c.o
LSO_Baldini: CMakeFiles/LSO_Baldini.dir/src/tester.c.o
LSO_Baldini: CMakeFiles/LSO_Baldini.dir/src/serverapi.c.o
LSO_Baldini: CMakeFiles/LSO_Baldini.dir/src/poll_utils.c.o
LSO_Baldini: CMakeFiles/LSO_Baldini.dir/src/dir_utils.c.o
LSO_Baldini: CMakeFiles/LSO_Baldini.dir/libs/LSO_LIB/array_utils.c.o
LSO_Baldini: CMakeFiles/LSO_Baldini.dir/libs/LSO_LIB/file_utils.c.o
LSO_Baldini: CMakeFiles/LSO_Baldini.dir/libs/LSO_LIB/hash_utils.c.o
LSO_Baldini: CMakeFiles/LSO_Baldini.dir/libs/LSO_LIB/list_utils.c.o
LSO_Baldini: CMakeFiles/LSO_Baldini.dir/libs/LSO_LIB/mem_utils.c.o
LSO_Baldini: CMakeFiles/LSO_Baldini.dir/libs/LSO_LIB/string_utils.c.o
LSO_Baldini: CMakeFiles/LSO_Baldini.dir/libs/LSO_LIB/thread_utils.c.o
LSO_Baldini: CMakeFiles/LSO_Baldini.dir/build.make
LSO_Baldini: CMakeFiles/LSO_Baldini.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/alberto/LSO_Baldini/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_14) "Linking C executable LSO_Baldini"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/LSO_Baldini.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/LSO_Baldini.dir/build: LSO_Baldini

.PHONY : CMakeFiles/LSO_Baldini.dir/build

CMakeFiles/LSO_Baldini.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/LSO_Baldini.dir/cmake_clean.cmake
.PHONY : CMakeFiles/LSO_Baldini.dir/clean

CMakeFiles/LSO_Baldini.dir/depend:
	cd /home/alberto/LSO_Baldini/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/alberto/LSO_Baldini /home/alberto/LSO_Baldini /home/alberto/LSO_Baldini/cmake-build-debug /home/alberto/LSO_Baldini/cmake-build-debug /home/alberto/LSO_Baldini/cmake-build-debug/CMakeFiles/LSO_Baldini.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/LSO_Baldini.dir/depend

