# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.13

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/mumumusuc/flutter_workspace/joycon/modules/bluez

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/mumumusuc/flutter_workspace/joycon/modules/bluez/build

# Include any dependencies generated for this target.
include CMakeFiles/bluez_util.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/bluez_util.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/bluez_util.dir/flags.make

CMakeFiles/bluez_util.dir/main.cc.o: CMakeFiles/bluez_util.dir/flags.make
CMakeFiles/bluez_util.dir/main.cc.o: ../main.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/mumumusuc/flutter_workspace/joycon/modules/bluez/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/bluez_util.dir/main.cc.o"
	/usr/bin/clang++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/bluez_util.dir/main.cc.o -c /home/mumumusuc/flutter_workspace/joycon/modules/bluez/main.cc

CMakeFiles/bluez_util.dir/main.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/bluez_util.dir/main.cc.i"
	/usr/bin/clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/mumumusuc/flutter_workspace/joycon/modules/bluez/main.cc > CMakeFiles/bluez_util.dir/main.cc.i

CMakeFiles/bluez_util.dir/main.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/bluez_util.dir/main.cc.s"
	/usr/bin/clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/mumumusuc/flutter_workspace/joycon/modules/bluez/main.cc -o CMakeFiles/bluez_util.dir/main.cc.s

CMakeFiles/bluez_util.dir/util.cc.o: CMakeFiles/bluez_util.dir/flags.make
CMakeFiles/bluez_util.dir/util.cc.o: ../util.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/mumumusuc/flutter_workspace/joycon/modules/bluez/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/bluez_util.dir/util.cc.o"
	/usr/bin/clang++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/bluez_util.dir/util.cc.o -c /home/mumumusuc/flutter_workspace/joycon/modules/bluez/util.cc

CMakeFiles/bluez_util.dir/util.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/bluez_util.dir/util.cc.i"
	/usr/bin/clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/mumumusuc/flutter_workspace/joycon/modules/bluez/util.cc > CMakeFiles/bluez_util.dir/util.cc.i

CMakeFiles/bluez_util.dir/util.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/bluez_util.dir/util.cc.s"
	/usr/bin/clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/mumumusuc/flutter_workspace/joycon/modules/bluez/util.cc -o CMakeFiles/bluez_util.dir/util.cc.s

# Object files for target bluez_util
bluez_util_OBJECTS = \
"CMakeFiles/bluez_util.dir/main.cc.o" \
"CMakeFiles/bluez_util.dir/util.cc.o"

# External object files for target bluez_util
bluez_util_EXTERNAL_OBJECTS =

bluez_util: CMakeFiles/bluez_util.dir/main.cc.o
bluez_util: CMakeFiles/bluez_util.dir/util.cc.o
bluez_util: CMakeFiles/bluez_util.dir/build.make
bluez_util: CMakeFiles/bluez_util.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/mumumusuc/flutter_workspace/joycon/modules/bluez/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX executable bluez_util"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/bluez_util.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/bluez_util.dir/build: bluez_util

.PHONY : CMakeFiles/bluez_util.dir/build

CMakeFiles/bluez_util.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/bluez_util.dir/cmake_clean.cmake
.PHONY : CMakeFiles/bluez_util.dir/clean

CMakeFiles/bluez_util.dir/depend:
	cd /home/mumumusuc/flutter_workspace/joycon/modules/bluez/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/mumumusuc/flutter_workspace/joycon/modules/bluez /home/mumumusuc/flutter_workspace/joycon/modules/bluez /home/mumumusuc/flutter_workspace/joycon/modules/bluez/build /home/mumumusuc/flutter_workspace/joycon/modules/bluez/build /home/mumumusuc/flutter_workspace/joycon/modules/bluez/build/CMakeFiles/bluez_util.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/bluez_util.dir/depend

