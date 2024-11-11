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

# Include any dependencies generated for this target.
include esp-idf/espressif__esp_audio_codec/CMakeFiles/__idf_espressif__esp_audio_codec.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include esp-idf/espressif__esp_audio_codec/CMakeFiles/__idf_espressif__esp_audio_codec.dir/compiler_depend.make

# Include the progress variables for this target.
include esp-idf/espressif__esp_audio_codec/CMakeFiles/__idf_espressif__esp_audio_codec.dir/progress.make

# Include the compile flags for this target's objects.
include esp-idf/espressif__esp_audio_codec/CMakeFiles/__idf_espressif__esp_audio_codec.dir/flags.make

esp-idf/espressif__esp_audio_codec/CMakeFiles/__idf_espressif__esp_audio_codec.dir/src/audio_decoder_reg.c.obj: esp-idf/espressif__esp_audio_codec/CMakeFiles/__idf_espressif__esp_audio_codec.dir/flags.make
esp-idf/espressif__esp_audio_codec/CMakeFiles/__idf_espressif__esp_audio_codec.dir/src/audio_decoder_reg.c.obj: /home/thien-gay/esp-idf/esp32-webRTC/managed_components/espressif__esp_audio_codec/src/audio_decoder_reg.c
esp-idf/espressif__esp_audio_codec/CMakeFiles/__idf_espressif__esp_audio_codec.dir/src/audio_decoder_reg.c.obj: esp-idf/espressif__esp_audio_codec/CMakeFiles/__idf_espressif__esp_audio_codec.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/thien-gay/esp-idf/esp32-webRTC/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object esp-idf/espressif__esp_audio_codec/CMakeFiles/__idf_espressif__esp_audio_codec.dir/src/audio_decoder_reg.c.obj"
	cd /home/thien-gay/esp-idf/esp32-webRTC/build/esp-idf/espressif__esp_audio_codec && /home/thien-gay/.espressif/tools/xtensa-esp-elf/esp-13.2.0_20230928/xtensa-esp-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT esp-idf/espressif__esp_audio_codec/CMakeFiles/__idf_espressif__esp_audio_codec.dir/src/audio_decoder_reg.c.obj -MF CMakeFiles/__idf_espressif__esp_audio_codec.dir/src/audio_decoder_reg.c.obj.d -o CMakeFiles/__idf_espressif__esp_audio_codec.dir/src/audio_decoder_reg.c.obj -c /home/thien-gay/esp-idf/esp32-webRTC/managed_components/espressif__esp_audio_codec/src/audio_decoder_reg.c

esp-idf/espressif__esp_audio_codec/CMakeFiles/__idf_espressif__esp_audio_codec.dir/src/audio_decoder_reg.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/__idf_espressif__esp_audio_codec.dir/src/audio_decoder_reg.c.i"
	cd /home/thien-gay/esp-idf/esp32-webRTC/build/esp-idf/espressif__esp_audio_codec && /home/thien-gay/.espressif/tools/xtensa-esp-elf/esp-13.2.0_20230928/xtensa-esp-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/thien-gay/esp-idf/esp32-webRTC/managed_components/espressif__esp_audio_codec/src/audio_decoder_reg.c > CMakeFiles/__idf_espressif__esp_audio_codec.dir/src/audio_decoder_reg.c.i

esp-idf/espressif__esp_audio_codec/CMakeFiles/__idf_espressif__esp_audio_codec.dir/src/audio_decoder_reg.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/__idf_espressif__esp_audio_codec.dir/src/audio_decoder_reg.c.s"
	cd /home/thien-gay/esp-idf/esp32-webRTC/build/esp-idf/espressif__esp_audio_codec && /home/thien-gay/.espressif/tools/xtensa-esp-elf/esp-13.2.0_20230928/xtensa-esp-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/thien-gay/esp-idf/esp32-webRTC/managed_components/espressif__esp_audio_codec/src/audio_decoder_reg.c -o CMakeFiles/__idf_espressif__esp_audio_codec.dir/src/audio_decoder_reg.c.s

esp-idf/espressif__esp_audio_codec/CMakeFiles/__idf_espressif__esp_audio_codec.dir/src/audio_encoder_reg.c.obj: esp-idf/espressif__esp_audio_codec/CMakeFiles/__idf_espressif__esp_audio_codec.dir/flags.make
esp-idf/espressif__esp_audio_codec/CMakeFiles/__idf_espressif__esp_audio_codec.dir/src/audio_encoder_reg.c.obj: /home/thien-gay/esp-idf/esp32-webRTC/managed_components/espressif__esp_audio_codec/src/audio_encoder_reg.c
esp-idf/espressif__esp_audio_codec/CMakeFiles/__idf_espressif__esp_audio_codec.dir/src/audio_encoder_reg.c.obj: esp-idf/espressif__esp_audio_codec/CMakeFiles/__idf_espressif__esp_audio_codec.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/thien-gay/esp-idf/esp32-webRTC/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object esp-idf/espressif__esp_audio_codec/CMakeFiles/__idf_espressif__esp_audio_codec.dir/src/audio_encoder_reg.c.obj"
	cd /home/thien-gay/esp-idf/esp32-webRTC/build/esp-idf/espressif__esp_audio_codec && /home/thien-gay/.espressif/tools/xtensa-esp-elf/esp-13.2.0_20230928/xtensa-esp-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT esp-idf/espressif__esp_audio_codec/CMakeFiles/__idf_espressif__esp_audio_codec.dir/src/audio_encoder_reg.c.obj -MF CMakeFiles/__idf_espressif__esp_audio_codec.dir/src/audio_encoder_reg.c.obj.d -o CMakeFiles/__idf_espressif__esp_audio_codec.dir/src/audio_encoder_reg.c.obj -c /home/thien-gay/esp-idf/esp32-webRTC/managed_components/espressif__esp_audio_codec/src/audio_encoder_reg.c

esp-idf/espressif__esp_audio_codec/CMakeFiles/__idf_espressif__esp_audio_codec.dir/src/audio_encoder_reg.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/__idf_espressif__esp_audio_codec.dir/src/audio_encoder_reg.c.i"
	cd /home/thien-gay/esp-idf/esp32-webRTC/build/esp-idf/espressif__esp_audio_codec && /home/thien-gay/.espressif/tools/xtensa-esp-elf/esp-13.2.0_20230928/xtensa-esp-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/thien-gay/esp-idf/esp32-webRTC/managed_components/espressif__esp_audio_codec/src/audio_encoder_reg.c > CMakeFiles/__idf_espressif__esp_audio_codec.dir/src/audio_encoder_reg.c.i

esp-idf/espressif__esp_audio_codec/CMakeFiles/__idf_espressif__esp_audio_codec.dir/src/audio_encoder_reg.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/__idf_espressif__esp_audio_codec.dir/src/audio_encoder_reg.c.s"
	cd /home/thien-gay/esp-idf/esp32-webRTC/build/esp-idf/espressif__esp_audio_codec && /home/thien-gay/.espressif/tools/xtensa-esp-elf/esp-13.2.0_20230928/xtensa-esp-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/thien-gay/esp-idf/esp32-webRTC/managed_components/espressif__esp_audio_codec/src/audio_encoder_reg.c -o CMakeFiles/__idf_espressif__esp_audio_codec.dir/src/audio_encoder_reg.c.s

esp-idf/espressif__esp_audio_codec/CMakeFiles/__idf_espressif__esp_audio_codec.dir/src/simple_decoder_reg.c.obj: esp-idf/espressif__esp_audio_codec/CMakeFiles/__idf_espressif__esp_audio_codec.dir/flags.make
esp-idf/espressif__esp_audio_codec/CMakeFiles/__idf_espressif__esp_audio_codec.dir/src/simple_decoder_reg.c.obj: /home/thien-gay/esp-idf/esp32-webRTC/managed_components/espressif__esp_audio_codec/src/simple_decoder_reg.c
esp-idf/espressif__esp_audio_codec/CMakeFiles/__idf_espressif__esp_audio_codec.dir/src/simple_decoder_reg.c.obj: esp-idf/espressif__esp_audio_codec/CMakeFiles/__idf_espressif__esp_audio_codec.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/thien-gay/esp-idf/esp32-webRTC/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building C object esp-idf/espressif__esp_audio_codec/CMakeFiles/__idf_espressif__esp_audio_codec.dir/src/simple_decoder_reg.c.obj"
	cd /home/thien-gay/esp-idf/esp32-webRTC/build/esp-idf/espressif__esp_audio_codec && /home/thien-gay/.espressif/tools/xtensa-esp-elf/esp-13.2.0_20230928/xtensa-esp-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT esp-idf/espressif__esp_audio_codec/CMakeFiles/__idf_espressif__esp_audio_codec.dir/src/simple_decoder_reg.c.obj -MF CMakeFiles/__idf_espressif__esp_audio_codec.dir/src/simple_decoder_reg.c.obj.d -o CMakeFiles/__idf_espressif__esp_audio_codec.dir/src/simple_decoder_reg.c.obj -c /home/thien-gay/esp-idf/esp32-webRTC/managed_components/espressif__esp_audio_codec/src/simple_decoder_reg.c

esp-idf/espressif__esp_audio_codec/CMakeFiles/__idf_espressif__esp_audio_codec.dir/src/simple_decoder_reg.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/__idf_espressif__esp_audio_codec.dir/src/simple_decoder_reg.c.i"
	cd /home/thien-gay/esp-idf/esp32-webRTC/build/esp-idf/espressif__esp_audio_codec && /home/thien-gay/.espressif/tools/xtensa-esp-elf/esp-13.2.0_20230928/xtensa-esp-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/thien-gay/esp-idf/esp32-webRTC/managed_components/espressif__esp_audio_codec/src/simple_decoder_reg.c > CMakeFiles/__idf_espressif__esp_audio_codec.dir/src/simple_decoder_reg.c.i

esp-idf/espressif__esp_audio_codec/CMakeFiles/__idf_espressif__esp_audio_codec.dir/src/simple_decoder_reg.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/__idf_espressif__esp_audio_codec.dir/src/simple_decoder_reg.c.s"
	cd /home/thien-gay/esp-idf/esp32-webRTC/build/esp-idf/espressif__esp_audio_codec && /home/thien-gay/.espressif/tools/xtensa-esp-elf/esp-13.2.0_20230928/xtensa-esp-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/thien-gay/esp-idf/esp32-webRTC/managed_components/espressif__esp_audio_codec/src/simple_decoder_reg.c -o CMakeFiles/__idf_espressif__esp_audio_codec.dir/src/simple_decoder_reg.c.s

# Object files for target __idf_espressif__esp_audio_codec
__idf_espressif__esp_audio_codec_OBJECTS = \
"CMakeFiles/__idf_espressif__esp_audio_codec.dir/src/audio_decoder_reg.c.obj" \
"CMakeFiles/__idf_espressif__esp_audio_codec.dir/src/audio_encoder_reg.c.obj" \
"CMakeFiles/__idf_espressif__esp_audio_codec.dir/src/simple_decoder_reg.c.obj"

# External object files for target __idf_espressif__esp_audio_codec
__idf_espressif__esp_audio_codec_EXTERNAL_OBJECTS =

esp-idf/espressif__esp_audio_codec/libespressif__esp_audio_codec.a: esp-idf/espressif__esp_audio_codec/CMakeFiles/__idf_espressif__esp_audio_codec.dir/src/audio_decoder_reg.c.obj
esp-idf/espressif__esp_audio_codec/libespressif__esp_audio_codec.a: esp-idf/espressif__esp_audio_codec/CMakeFiles/__idf_espressif__esp_audio_codec.dir/src/audio_encoder_reg.c.obj
esp-idf/espressif__esp_audio_codec/libespressif__esp_audio_codec.a: esp-idf/espressif__esp_audio_codec/CMakeFiles/__idf_espressif__esp_audio_codec.dir/src/simple_decoder_reg.c.obj
esp-idf/espressif__esp_audio_codec/libespressif__esp_audio_codec.a: esp-idf/espressif__esp_audio_codec/CMakeFiles/__idf_espressif__esp_audio_codec.dir/build.make
esp-idf/espressif__esp_audio_codec/libespressif__esp_audio_codec.a: esp-idf/espressif__esp_audio_codec/CMakeFiles/__idf_espressif__esp_audio_codec.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/home/thien-gay/esp-idf/esp32-webRTC/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Linking C static library libespressif__esp_audio_codec.a"
	cd /home/thien-gay/esp-idf/esp32-webRTC/build/esp-idf/espressif__esp_audio_codec && $(CMAKE_COMMAND) -P CMakeFiles/__idf_espressif__esp_audio_codec.dir/cmake_clean_target.cmake
	cd /home/thien-gay/esp-idf/esp32-webRTC/build/esp-idf/espressif__esp_audio_codec && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/__idf_espressif__esp_audio_codec.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
esp-idf/espressif__esp_audio_codec/CMakeFiles/__idf_espressif__esp_audio_codec.dir/build: esp-idf/espressif__esp_audio_codec/libespressif__esp_audio_codec.a
.PHONY : esp-idf/espressif__esp_audio_codec/CMakeFiles/__idf_espressif__esp_audio_codec.dir/build

esp-idf/espressif__esp_audio_codec/CMakeFiles/__idf_espressif__esp_audio_codec.dir/clean:
	cd /home/thien-gay/esp-idf/esp32-webRTC/build/esp-idf/espressif__esp_audio_codec && $(CMAKE_COMMAND) -P CMakeFiles/__idf_espressif__esp_audio_codec.dir/cmake_clean.cmake
.PHONY : esp-idf/espressif__esp_audio_codec/CMakeFiles/__idf_espressif__esp_audio_codec.dir/clean

esp-idf/espressif__esp_audio_codec/CMakeFiles/__idf_espressif__esp_audio_codec.dir/depend:
	cd /home/thien-gay/esp-idf/esp32-webRTC/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/thien-gay/esp-idf/esp32-webRTC /home/thien-gay/esp-idf/esp32-webRTC/managed_components/espressif__esp_audio_codec /home/thien-gay/esp-idf/esp32-webRTC/build /home/thien-gay/esp-idf/esp32-webRTC/build/esp-idf/espressif__esp_audio_codec /home/thien-gay/esp-idf/esp32-webRTC/build/esp-idf/espressif__esp_audio_codec/CMakeFiles/__idf_espressif__esp_audio_codec.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : esp-idf/espressif__esp_audio_codec/CMakeFiles/__idf_espressif__esp_audio_codec.dir/depend
