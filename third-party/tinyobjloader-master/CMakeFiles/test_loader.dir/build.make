# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.8

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list

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
CMAKE_COMMAND = /opt/apps/cmake/2.8.12.2/bin/cmake

# The command to remove a file.
RM = /opt/apps/cmake/2.8.12.2/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The program to use to edit the cache.
CMAKE_EDIT_COMMAND = /opt/apps/cmake/2.8.12.2/bin/ccmake

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/01197/semeraro/Projects/Gravity/gravit/third-party/tinyobjloader-master

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/01197/semeraro/Projects/Gravity/gravit/third-party/tinyobjloader-master

# Include any dependencies generated for this target.
include CMakeFiles/test_loader.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/test_loader.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/test_loader.dir/flags.make

CMakeFiles/test_loader.dir/test.cc.o: CMakeFiles/test_loader.dir/flags.make
CMakeFiles/test_loader.dir/test.cc.o: test.cc
	$(CMAKE_COMMAND) -E cmake_progress_report /home/01197/semeraro/Projects/Gravity/gravit/third-party/tinyobjloader-master/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/test_loader.dir/test.cc.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/test_loader.dir/test.cc.o -c /home/01197/semeraro/Projects/Gravity/gravit/third-party/tinyobjloader-master/test.cc

CMakeFiles/test_loader.dir/test.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/test_loader.dir/test.cc.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/01197/semeraro/Projects/Gravity/gravit/third-party/tinyobjloader-master/test.cc > CMakeFiles/test_loader.dir/test.cc.i

CMakeFiles/test_loader.dir/test.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/test_loader.dir/test.cc.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/01197/semeraro/Projects/Gravity/gravit/third-party/tinyobjloader-master/test.cc -o CMakeFiles/test_loader.dir/test.cc.s

CMakeFiles/test_loader.dir/test.cc.o.requires:
.PHONY : CMakeFiles/test_loader.dir/test.cc.o.requires

CMakeFiles/test_loader.dir/test.cc.o.provides: CMakeFiles/test_loader.dir/test.cc.o.requires
	$(MAKE) -f CMakeFiles/test_loader.dir/build.make CMakeFiles/test_loader.dir/test.cc.o.provides.build
.PHONY : CMakeFiles/test_loader.dir/test.cc.o.provides

CMakeFiles/test_loader.dir/test.cc.o.provides.build: CMakeFiles/test_loader.dir/test.cc.o

# Object files for target test_loader
test_loader_OBJECTS = \
"CMakeFiles/test_loader.dir/test.cc.o"

# External object files for target test_loader
test_loader_EXTERNAL_OBJECTS =

test_loader: CMakeFiles/test_loader.dir/test.cc.o
test_loader: CMakeFiles/test_loader.dir/build.make
test_loader: libtinyobjloader.a
test_loader: CMakeFiles/test_loader.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX executable test_loader"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/test_loader.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/test_loader.dir/build: test_loader
.PHONY : CMakeFiles/test_loader.dir/build

CMakeFiles/test_loader.dir/requires: CMakeFiles/test_loader.dir/test.cc.o.requires
.PHONY : CMakeFiles/test_loader.dir/requires

CMakeFiles/test_loader.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/test_loader.dir/cmake_clean.cmake
.PHONY : CMakeFiles/test_loader.dir/clean

CMakeFiles/test_loader.dir/depend:
	cd /home/01197/semeraro/Projects/Gravity/gravit/third-party/tinyobjloader-master && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/01197/semeraro/Projects/Gravity/gravit/third-party/tinyobjloader-master /home/01197/semeraro/Projects/Gravity/gravit/third-party/tinyobjloader-master /home/01197/semeraro/Projects/Gravity/gravit/third-party/tinyobjloader-master /home/01197/semeraro/Projects/Gravity/gravit/third-party/tinyobjloader-master /home/01197/semeraro/Projects/Gravity/gravit/third-party/tinyobjloader-master/CMakeFiles/test_loader.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/test_loader.dir/depend

