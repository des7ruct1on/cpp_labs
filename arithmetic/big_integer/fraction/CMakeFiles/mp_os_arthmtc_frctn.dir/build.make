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
CMAKE_COMMAND = /opt/homebrew/Cellar/cmake/3.28.3/bin/cmake

# The command to remove a file.
RM = /opt/homebrew/Cellar/cmake/3.28.3/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/mollyinmycup/Desktop/mp_os

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/mollyinmycup/Desktop/mp_os

# Include any dependencies generated for this target.
include arithmetic/fraction/CMakeFiles/mp_os_arthmtc_frctn.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include arithmetic/fraction/CMakeFiles/mp_os_arthmtc_frctn.dir/compiler_depend.make

# Include the progress variables for this target.
include arithmetic/fraction/CMakeFiles/mp_os_arthmtc_frctn.dir/progress.make

# Include the compile flags for this target's objects.
include arithmetic/fraction/CMakeFiles/mp_os_arthmtc_frctn.dir/flags.make

arithmetic/fraction/CMakeFiles/mp_os_arthmtc_frctn.dir/src/fraction.cpp.o: arithmetic/fraction/CMakeFiles/mp_os_arthmtc_frctn.dir/flags.make
arithmetic/fraction/CMakeFiles/mp_os_arthmtc_frctn.dir/src/fraction.cpp.o: arithmetic/fraction/src/fraction.cpp
arithmetic/fraction/CMakeFiles/mp_os_arthmtc_frctn.dir/src/fraction.cpp.o: arithmetic/fraction/CMakeFiles/mp_os_arthmtc_frctn.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/Users/mollyinmycup/Desktop/mp_os/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object arithmetic/fraction/CMakeFiles/mp_os_arthmtc_frctn.dir/src/fraction.cpp.o"
	cd /Users/mollyinmycup/Desktop/mp_os/arithmetic/fraction && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT arithmetic/fraction/CMakeFiles/mp_os_arthmtc_frctn.dir/src/fraction.cpp.o -MF CMakeFiles/mp_os_arthmtc_frctn.dir/src/fraction.cpp.o.d -o CMakeFiles/mp_os_arthmtc_frctn.dir/src/fraction.cpp.o -c /Users/mollyinmycup/Desktop/mp_os/arithmetic/fraction/src/fraction.cpp

arithmetic/fraction/CMakeFiles/mp_os_arthmtc_frctn.dir/src/fraction.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/mp_os_arthmtc_frctn.dir/src/fraction.cpp.i"
	cd /Users/mollyinmycup/Desktop/mp_os/arithmetic/fraction && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/mollyinmycup/Desktop/mp_os/arithmetic/fraction/src/fraction.cpp > CMakeFiles/mp_os_arthmtc_frctn.dir/src/fraction.cpp.i

arithmetic/fraction/CMakeFiles/mp_os_arthmtc_frctn.dir/src/fraction.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/mp_os_arthmtc_frctn.dir/src/fraction.cpp.s"
	cd /Users/mollyinmycup/Desktop/mp_os/arithmetic/fraction && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/mollyinmycup/Desktop/mp_os/arithmetic/fraction/src/fraction.cpp -o CMakeFiles/mp_os_arthmtc_frctn.dir/src/fraction.cpp.s

# Object files for target mp_os_arthmtc_frctn
mp_os_arthmtc_frctn_OBJECTS = \
"CMakeFiles/mp_os_arthmtc_frctn.dir/src/fraction.cpp.o"

# External object files for target mp_os_arthmtc_frctn
mp_os_arthmtc_frctn_EXTERNAL_OBJECTS =

arithmetic/fraction/libmp_os_arthmtc_frctn.1.0.dylib: arithmetic/fraction/CMakeFiles/mp_os_arthmtc_frctn.dir/src/fraction.cpp.o
arithmetic/fraction/libmp_os_arthmtc_frctn.1.0.dylib: arithmetic/fraction/CMakeFiles/mp_os_arthmtc_frctn.dir/build.make
arithmetic/fraction/libmp_os_arthmtc_frctn.1.0.dylib: common/libmp_os_cmmn.a
arithmetic/fraction/libmp_os_arthmtc_frctn.1.0.dylib: allocator/allocator/libmp_os_allctr_allctr.a
arithmetic/fraction/libmp_os_arthmtc_frctn.1.0.dylib: arithmetic/big_integer/libmp_os_arthmtc_bg_intgr.a
arithmetic/fraction/libmp_os_arthmtc_frctn.1.0.dylib: common/libmp_os_cmmn.a
arithmetic/fraction/libmp_os_arthmtc_frctn.1.0.dylib: allocator/allocator/libmp_os_allctr_allctr.a
arithmetic/fraction/libmp_os_arthmtc_frctn.1.0.dylib: arithmetic/fraction/CMakeFiles/mp_os_arthmtc_frctn.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/Users/mollyinmycup/Desktop/mp_os/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX shared library libmp_os_arthmtc_frctn.dylib"
	cd /Users/mollyinmycup/Desktop/mp_os/arithmetic/fraction && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/mp_os_arthmtc_frctn.dir/link.txt --verbose=$(VERBOSE)
	cd /Users/mollyinmycup/Desktop/mp_os/arithmetic/fraction && $(CMAKE_COMMAND) -E cmake_symlink_library libmp_os_arthmtc_frctn.1.0.dylib libmp_os_arthmtc_frctn.1.0.dylib libmp_os_arthmtc_frctn.dylib

arithmetic/fraction/libmp_os_arthmtc_frctn.dylib: arithmetic/fraction/libmp_os_arthmtc_frctn.1.0.dylib
	@$(CMAKE_COMMAND) -E touch_nocreate arithmetic/fraction/libmp_os_arthmtc_frctn.dylib

# Rule to build all files generated by this target.
arithmetic/fraction/CMakeFiles/mp_os_arthmtc_frctn.dir/build: arithmetic/fraction/libmp_os_arthmtc_frctn.dylib
.PHONY : arithmetic/fraction/CMakeFiles/mp_os_arthmtc_frctn.dir/build

arithmetic/fraction/CMakeFiles/mp_os_arthmtc_frctn.dir/clean:
	cd /Users/mollyinmycup/Desktop/mp_os/arithmetic/fraction && $(CMAKE_COMMAND) -P CMakeFiles/mp_os_arthmtc_frctn.dir/cmake_clean.cmake
.PHONY : arithmetic/fraction/CMakeFiles/mp_os_arthmtc_frctn.dir/clean

arithmetic/fraction/CMakeFiles/mp_os_arthmtc_frctn.dir/depend:
	cd /Users/mollyinmycup/Desktop/mp_os && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/mollyinmycup/Desktop/mp_os /Users/mollyinmycup/Desktop/mp_os/arithmetic/fraction /Users/mollyinmycup/Desktop/mp_os /Users/mollyinmycup/Desktop/mp_os/arithmetic/fraction /Users/mollyinmycup/Desktop/mp_os/arithmetic/fraction/CMakeFiles/mp_os_arthmtc_frctn.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : arithmetic/fraction/CMakeFiles/mp_os_arthmtc_frctn.dir/depend

