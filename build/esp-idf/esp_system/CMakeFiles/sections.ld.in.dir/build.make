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
CMAKE_SOURCE_DIR = /home/thien-gay/esp-idf/esp32-webRTC

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/thien-gay/esp-idf/esp32-webRTC/build

# Utility rule file for sections.ld.in.

# Include any custom commands dependencies for this target.
include esp-idf/esp_system/CMakeFiles/sections.ld.in.dir/compiler_depend.make

# Include the progress variables for this target.
include esp-idf/esp_system/CMakeFiles/sections.ld.in.dir/progress.make

esp-idf/esp_system/CMakeFiles/sections.ld.in: esp-idf/esp_system/ld/sections.ld.in

esp-idf/esp_system/ld/sections.ld.in: /home/thien-gay/esp-idf/esp-idf/components/esp_system/ld/esp32/sections.ld.in
esp-idf/esp_system/ld/sections.ld.in: config/sdkconfig.h
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --blue --bold --progress-dir=/home/thien-gay/esp-idf/esp32-webRTC/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Generating /home/thien-gay/esp-idf/esp32-webRTC/build/esp-idf/esp_system/ld/sections.ld.in linker script..."
	cd /home/thien-gay/esp-idf/esp32-webRTC/build/esp-idf/esp_system && /usr/bin/cmake -DCC=/home/thien-gay/.espressif/tools/xtensa-esp-elf/esp-13.2.0_20230928/xtensa-esp-elf/bin/xtensa-esp32-elf-gcc -DSOURCE=/home/thien-gay/esp-idf/esp-idf/components/esp_system/ld/esp32/sections.ld.in -DTARGET=/home/thien-gay/esp-idf/esp32-webRTC/build/esp-idf/esp_system/ld/sections.ld.in -DCONFIG_DIR=/home/thien-gay/esp-idf/esp32-webRTC/build/config -DLD_DIR=/home/thien-gay/esp-idf/esp-idf/components/esp_system/ld -P /home/thien-gay/esp-idf/esp32-webRTC/build/esp-idf/esp_system/ld/linker_script_generator.cmake

sections.ld.in: esp-idf/esp_system/CMakeFiles/sections.ld.in
sections.ld.in: esp-idf/esp_system/ld/sections.ld.in
sections.ld.in: esp-idf/esp_system/CMakeFiles/sections.ld.in.dir/build.make
.PHONY : sections.ld.in

# Rule to build all files generated by this target.
esp-idf/esp_system/CMakeFiles/sections.ld.in.dir/build: sections.ld.in
.PHONY : esp-idf/esp_system/CMakeFiles/sections.ld.in.dir/build

esp-idf/esp_system/CMakeFiles/sections.ld.in.dir/clean:
	cd /home/thien-gay/esp-idf/esp32-webRTC/build/esp-idf/esp_system && $(CMAKE_COMMAND) -P CMakeFiles/sections.ld.in.dir/cmake_clean.cmake
.PHONY : esp-idf/esp_system/CMakeFiles/sections.ld.in.dir/clean

esp-idf/esp_system/CMakeFiles/sections.ld.in.dir/depend:
	cd /home/thien-gay/esp-idf/esp32-webRTC/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/thien-gay/esp-idf/esp32-webRTC /home/thien-gay/esp-idf/esp-idf/components/esp_system /home/thien-gay/esp-idf/esp32-webRTC/build /home/thien-gay/esp-idf/esp32-webRTC/build/esp-idf/esp_system /home/thien-gay/esp-idf/esp32-webRTC/build/esp-idf/esp_system/CMakeFiles/sections.ld.in.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : esp-idf/esp_system/CMakeFiles/sections.ld.in.dir/depend
