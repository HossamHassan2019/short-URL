# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.28

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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/hossam/Desktop/short-URL

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/hossam/Desktop/short-URL/build

# Utility rule file for uninstall.

# Include any custom commands dependencies for this target.
include external/crow/CMakeFiles/uninstall.dir/compiler_depend.make

# Include the progress variables for this target.
include external/crow/CMakeFiles/uninstall.dir/progress.make

external/crow/CMakeFiles/uninstall:
	cd /home/hossam/Desktop/short-URL/build/external/crow && /usr/bin/cmake -P /home/hossam/Desktop/short-URL/build/external/crow/cmake_uninstall.cmake

uninstall: external/crow/CMakeFiles/uninstall
uninstall: external/crow/CMakeFiles/uninstall.dir/build.make
.PHONY : uninstall

# Rule to build all files generated by this target.
external/crow/CMakeFiles/uninstall.dir/build: uninstall
.PHONY : external/crow/CMakeFiles/uninstall.dir/build

external/crow/CMakeFiles/uninstall.dir/clean:
	cd /home/hossam/Desktop/short-URL/build/external/crow && $(CMAKE_COMMAND) -P CMakeFiles/uninstall.dir/cmake_clean.cmake
.PHONY : external/crow/CMakeFiles/uninstall.dir/clean

external/crow/CMakeFiles/uninstall.dir/depend:
	cd /home/hossam/Desktop/short-URL/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/hossam/Desktop/short-URL /home/hossam/Desktop/short-URL/external/crow /home/hossam/Desktop/short-URL/build /home/hossam/Desktop/short-URL/build/external/crow /home/hossam/Desktop/short-URL/build/external/crow/CMakeFiles/uninstall.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : external/crow/CMakeFiles/uninstall.dir/depend

