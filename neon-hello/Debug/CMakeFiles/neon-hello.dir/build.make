# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.5

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Produce verbose output by default.
VERBOSE = 1

# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/hinoeng/workspace/Eclipse/neon-hello

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/hinoeng/workspace/Eclipse/neon-hello/Debug

# Include any dependencies generated for this target.
include CMakeFiles/neon-hello.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/neon-hello.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/neon-hello.dir/flags.make

CMakeFiles/neon-hello.dir/src/main.cpp.o: CMakeFiles/neon-hello.dir/flags.make
CMakeFiles/neon-hello.dir/src/main.cpp.o: ../src/main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/hinoeng/workspace/Eclipse/neon-hello/Debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/neon-hello.dir/src/main.cpp.o"
	/opt/fsl-imx-fb/4.1.15-1.1.1/sysroots/x86_64-pokysdk-linux/usr/bin/arm-poky-linux-gnueabi/arm-poky-linux-gnueabi-g++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/neon-hello.dir/src/main.cpp.o -c /home/hinoeng/workspace/Eclipse/neon-hello/src/main.cpp

CMakeFiles/neon-hello.dir/src/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/neon-hello.dir/src/main.cpp.i"
	/opt/fsl-imx-fb/4.1.15-1.1.1/sysroots/x86_64-pokysdk-linux/usr/bin/arm-poky-linux-gnueabi/arm-poky-linux-gnueabi-g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/hinoeng/workspace/Eclipse/neon-hello/src/main.cpp > CMakeFiles/neon-hello.dir/src/main.cpp.i

CMakeFiles/neon-hello.dir/src/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/neon-hello.dir/src/main.cpp.s"
	/opt/fsl-imx-fb/4.1.15-1.1.1/sysroots/x86_64-pokysdk-linux/usr/bin/arm-poky-linux-gnueabi/arm-poky-linux-gnueabi-g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/hinoeng/workspace/Eclipse/neon-hello/src/main.cpp -o CMakeFiles/neon-hello.dir/src/main.cpp.s

CMakeFiles/neon-hello.dir/src/main.cpp.o.requires:

.PHONY : CMakeFiles/neon-hello.dir/src/main.cpp.o.requires

CMakeFiles/neon-hello.dir/src/main.cpp.o.provides: CMakeFiles/neon-hello.dir/src/main.cpp.o.requires
	$(MAKE) -f CMakeFiles/neon-hello.dir/build.make CMakeFiles/neon-hello.dir/src/main.cpp.o.provides.build
.PHONY : CMakeFiles/neon-hello.dir/src/main.cpp.o.provides

CMakeFiles/neon-hello.dir/src/main.cpp.o.provides.build: CMakeFiles/neon-hello.dir/src/main.cpp.o


# Object files for target neon-hello
neon__hello_OBJECTS = \
"CMakeFiles/neon-hello.dir/src/main.cpp.o"

# External object files for target neon-hello
neon__hello_EXTERNAL_OBJECTS =

neon-hello: CMakeFiles/neon-hello.dir/src/main.cpp.o
neon-hello: CMakeFiles/neon-hello.dir/build.make
neon-hello: CMakeFiles/neon-hello.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/hinoeng/workspace/Eclipse/neon-hello/Debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable neon-hello"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/neon-hello.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/neon-hello.dir/build: neon-hello

.PHONY : CMakeFiles/neon-hello.dir/build

CMakeFiles/neon-hello.dir/requires: CMakeFiles/neon-hello.dir/src/main.cpp.o.requires

.PHONY : CMakeFiles/neon-hello.dir/requires

CMakeFiles/neon-hello.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/neon-hello.dir/cmake_clean.cmake
.PHONY : CMakeFiles/neon-hello.dir/clean

CMakeFiles/neon-hello.dir/depend:
	cd /home/hinoeng/workspace/Eclipse/neon-hello/Debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/hinoeng/workspace/Eclipse/neon-hello /home/hinoeng/workspace/Eclipse/neon-hello /home/hinoeng/workspace/Eclipse/neon-hello/Debug /home/hinoeng/workspace/Eclipse/neon-hello/Debug /home/hinoeng/workspace/Eclipse/neon-hello/Debug/CMakeFiles/neon-hello.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/neon-hello.dir/depend

