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
include CMakeFiles/obj_sticher.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/obj_sticher.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/obj_sticher.dir/flags.make

CMakeFiles/obj_sticher.dir/examples/obj_sticher/obj_writer.cc.o: CMakeFiles/obj_sticher.dir/flags.make
CMakeFiles/obj_sticher.dir/examples/obj_sticher/obj_writer.cc.o: examples/obj_sticher/obj_writer.cc
	$(CMAKE_COMMAND) -E cmake_progress_report /home/01197/semeraro/Projects/Gravity/gravit/third-party/tinyobjloader-master/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/obj_sticher.dir/examples/obj_sticher/obj_writer.cc.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/obj_sticher.dir/examples/obj_sticher/obj_writer.cc.o -c /home/01197/semeraro/Projects/Gravity/gravit/third-party/tinyobjloader-master/examples/obj_sticher/obj_writer.cc

CMakeFiles/obj_sticher.dir/examples/obj_sticher/obj_writer.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/obj_sticher.dir/examples/obj_sticher/obj_writer.cc.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/01197/semeraro/Projects/Gravity/gravit/third-party/tinyobjloader-master/examples/obj_sticher/obj_writer.cc > CMakeFiles/obj_sticher.dir/examples/obj_sticher/obj_writer.cc.i

CMakeFiles/obj_sticher.dir/examples/obj_sticher/obj_writer.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/obj_sticher.dir/examples/obj_sticher/obj_writer.cc.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/01197/semeraro/Projects/Gravity/gravit/third-party/tinyobjloader-master/examples/obj_sticher/obj_writer.cc -o CMakeFiles/obj_sticher.dir/examples/obj_sticher/obj_writer.cc.s

CMakeFiles/obj_sticher.dir/examples/obj_sticher/obj_writer.cc.o.requires:
.PHONY : CMakeFiles/obj_sticher.dir/examples/obj_sticher/obj_writer.cc.o.requires

CMakeFiles/obj_sticher.dir/examples/obj_sticher/obj_writer.cc.o.provides: CMakeFiles/obj_sticher.dir/examples/obj_sticher/obj_writer.cc.o.requires
	$(MAKE) -f CMakeFiles/obj_sticher.dir/build.make CMakeFiles/obj_sticher.dir/examples/obj_sticher/obj_writer.cc.o.provides.build
.PHONY : CMakeFiles/obj_sticher.dir/examples/obj_sticher/obj_writer.cc.o.provides

CMakeFiles/obj_sticher.dir/examples/obj_sticher/obj_writer.cc.o.provides.build: CMakeFiles/obj_sticher.dir/examples/obj_sticher/obj_writer.cc.o

CMakeFiles/obj_sticher.dir/examples/obj_sticher/obj_sticher.cc.o: CMakeFiles/obj_sticher.dir/flags.make
CMakeFiles/obj_sticher.dir/examples/obj_sticher/obj_sticher.cc.o: examples/obj_sticher/obj_sticher.cc
	$(CMAKE_COMMAND) -E cmake_progress_report /home/01197/semeraro/Projects/Gravity/gravit/third-party/tinyobjloader-master/CMakeFiles $(CMAKE_PROGRESS_2)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/obj_sticher.dir/examples/obj_sticher/obj_sticher.cc.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/obj_sticher.dir/examples/obj_sticher/obj_sticher.cc.o -c /home/01197/semeraro/Projects/Gravity/gravit/third-party/tinyobjloader-master/examples/obj_sticher/obj_sticher.cc

CMakeFiles/obj_sticher.dir/examples/obj_sticher/obj_sticher.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/obj_sticher.dir/examples/obj_sticher/obj_sticher.cc.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/01197/semeraro/Projects/Gravity/gravit/third-party/tinyobjloader-master/examples/obj_sticher/obj_sticher.cc > CMakeFiles/obj_sticher.dir/examples/obj_sticher/obj_sticher.cc.i

CMakeFiles/obj_sticher.dir/examples/obj_sticher/obj_sticher.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/obj_sticher.dir/examples/obj_sticher/obj_sticher.cc.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/01197/semeraro/Projects/Gravity/gravit/third-party/tinyobjloader-master/examples/obj_sticher/obj_sticher.cc -o CMakeFiles/obj_sticher.dir/examples/obj_sticher/obj_sticher.cc.s

CMakeFiles/obj_sticher.dir/examples/obj_sticher/obj_sticher.cc.o.requires:
.PHONY : CMakeFiles/obj_sticher.dir/examples/obj_sticher/obj_sticher.cc.o.requires

CMakeFiles/obj_sticher.dir/examples/obj_sticher/obj_sticher.cc.o.provides: CMakeFiles/obj_sticher.dir/examples/obj_sticher/obj_sticher.cc.o.requires
	$(MAKE) -f CMakeFiles/obj_sticher.dir/build.make CMakeFiles/obj_sticher.dir/examples/obj_sticher/obj_sticher.cc.o.provides.build
.PHONY : CMakeFiles/obj_sticher.dir/examples/obj_sticher/obj_sticher.cc.o.provides

CMakeFiles/obj_sticher.dir/examples/obj_sticher/obj_sticher.cc.o.provides.build: CMakeFiles/obj_sticher.dir/examples/obj_sticher/obj_sticher.cc.o

# Object files for target obj_sticher
obj_sticher_OBJECTS = \
"CMakeFiles/obj_sticher.dir/examples/obj_sticher/obj_writer.cc.o" \
"CMakeFiles/obj_sticher.dir/examples/obj_sticher/obj_sticher.cc.o"

# External object files for target obj_sticher
obj_sticher_EXTERNAL_OBJECTS =

obj_sticher: CMakeFiles/obj_sticher.dir/examples/obj_sticher/obj_writer.cc.o
obj_sticher: CMakeFiles/obj_sticher.dir/examples/obj_sticher/obj_sticher.cc.o
obj_sticher: CMakeFiles/obj_sticher.dir/build.make
obj_sticher: libtinyobjloader.a
obj_sticher: CMakeFiles/obj_sticher.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX executable obj_sticher"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/obj_sticher.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/obj_sticher.dir/build: obj_sticher
.PHONY : CMakeFiles/obj_sticher.dir/build

CMakeFiles/obj_sticher.dir/requires: CMakeFiles/obj_sticher.dir/examples/obj_sticher/obj_writer.cc.o.requires
CMakeFiles/obj_sticher.dir/requires: CMakeFiles/obj_sticher.dir/examples/obj_sticher/obj_sticher.cc.o.requires
.PHONY : CMakeFiles/obj_sticher.dir/requires

CMakeFiles/obj_sticher.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/obj_sticher.dir/cmake_clean.cmake
.PHONY : CMakeFiles/obj_sticher.dir/clean

CMakeFiles/obj_sticher.dir/depend:
	cd /home/01197/semeraro/Projects/Gravity/gravit/third-party/tinyobjloader-master && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/01197/semeraro/Projects/Gravity/gravit/third-party/tinyobjloader-master /home/01197/semeraro/Projects/Gravity/gravit/third-party/tinyobjloader-master /home/01197/semeraro/Projects/Gravity/gravit/third-party/tinyobjloader-master /home/01197/semeraro/Projects/Gravity/gravit/third-party/tinyobjloader-master /home/01197/semeraro/Projects/Gravity/gravit/third-party/tinyobjloader-master/CMakeFiles/obj_sticher.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/obj_sticher.dir/depend

