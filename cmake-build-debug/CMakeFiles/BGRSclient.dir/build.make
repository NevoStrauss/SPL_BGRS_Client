# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.17

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
CMAKE_COMMAND = /home/nevo/Documents/clion-2020.2.4/bin/cmake/linux/bin/cmake

# The command to remove a file.
RM = /home/nevo/Documents/clion-2020.2.4/bin/cmake/linux/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/nevo/ronracheb/Client

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/nevo/ronracheb/Client/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/BGRSclient.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/BGRSclient.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/BGRSclient.dir/flags.make

CMakeFiles/BGRSclient.dir/src/BGRSclient.cpp.o: CMakeFiles/BGRSclient.dir/flags.make
CMakeFiles/BGRSclient.dir/src/BGRSclient.cpp.o: ../src/BGRSclient.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/nevo/ronracheb/Client/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/BGRSclient.dir/src/BGRSclient.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/BGRSclient.dir/src/BGRSclient.cpp.o -c /home/nevo/ronracheb/Client/src/BGRSclient.cpp

CMakeFiles/BGRSclient.dir/src/BGRSclient.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/BGRSclient.dir/src/BGRSclient.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/nevo/ronracheb/Client/src/BGRSclient.cpp > CMakeFiles/BGRSclient.dir/src/BGRSclient.cpp.i

CMakeFiles/BGRSclient.dir/src/BGRSclient.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/BGRSclient.dir/src/BGRSclient.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/nevo/ronracheb/Client/src/BGRSclient.cpp -o CMakeFiles/BGRSclient.dir/src/BGRSclient.cpp.s

CMakeFiles/BGRSclient.dir/src/connectionHandler.cpp.o: CMakeFiles/BGRSclient.dir/flags.make
CMakeFiles/BGRSclient.dir/src/connectionHandler.cpp.o: ../src/connectionHandler.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/nevo/ronracheb/Client/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/BGRSclient.dir/src/connectionHandler.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/BGRSclient.dir/src/connectionHandler.cpp.o -c /home/nevo/ronracheb/Client/src/connectionHandler.cpp

CMakeFiles/BGRSclient.dir/src/connectionHandler.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/BGRSclient.dir/src/connectionHandler.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/nevo/ronracheb/Client/src/connectionHandler.cpp > CMakeFiles/BGRSclient.dir/src/connectionHandler.cpp.i

CMakeFiles/BGRSclient.dir/src/connectionHandler.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/BGRSclient.dir/src/connectionHandler.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/nevo/ronracheb/Client/src/connectionHandler.cpp -o CMakeFiles/BGRSclient.dir/src/connectionHandler.cpp.s

# Object files for target BGRSclient
BGRSclient_OBJECTS = \
"CMakeFiles/BGRSclient.dir/src/BGRSclient.cpp.o" \
"CMakeFiles/BGRSclient.dir/src/connectionHandler.cpp.o"

# External object files for target BGRSclient
BGRSclient_EXTERNAL_OBJECTS =

BGRSclient: CMakeFiles/BGRSclient.dir/src/BGRSclient.cpp.o
BGRSclient: CMakeFiles/BGRSclient.dir/src/connectionHandler.cpp.o
BGRSclient: CMakeFiles/BGRSclient.dir/build.make
BGRSclient: CMakeFiles/BGRSclient.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/nevo/ronracheb/Client/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX executable BGRSclient"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/BGRSclient.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/BGRSclient.dir/build: BGRSclient

.PHONY : CMakeFiles/BGRSclient.dir/build

CMakeFiles/BGRSclient.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/BGRSclient.dir/cmake_clean.cmake
.PHONY : CMakeFiles/BGRSclient.dir/clean

CMakeFiles/BGRSclient.dir/depend:
	cd /home/nevo/ronracheb/Client/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/nevo/ronracheb/Client /home/nevo/ronracheb/Client /home/nevo/ronracheb/Client/cmake-build-debug /home/nevo/ronracheb/Client/cmake-build-debug /home/nevo/ronracheb/Client/cmake-build-debug/CMakeFiles/BGRSclient.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/BGRSclient.dir/depend

