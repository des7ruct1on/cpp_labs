# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.29

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
CMAKE_COMMAND = /opt/homebrew/Cellar/cmake/3.29.0/bin/cmake

# The command to remove a file.
RM = /opt/homebrew/Cellar/cmake/3.29.0/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/ivan/desktop/c++/cpp_labs

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/ivan/desktop/c++/cpp_labs

# Include any dependencies generated for this target.
include logger/client_logger/tests/CMakeFiles/mp_os_lggr_clnt_lggr_tests.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include logger/client_logger/tests/CMakeFiles/mp_os_lggr_clnt_lggr_tests.dir/compiler_depend.make

# Include the progress variables for this target.
include logger/client_logger/tests/CMakeFiles/mp_os_lggr_clnt_lggr_tests.dir/progress.make

# Include the compile flags for this target's objects.
include logger/client_logger/tests/CMakeFiles/mp_os_lggr_clnt_lggr_tests.dir/flags.make

logger/client_logger/tests/CMakeFiles/mp_os_lggr_clnt_lggr_tests.dir/client_logger_tests.cpp.o: logger/client_logger/tests/CMakeFiles/mp_os_lggr_clnt_lggr_tests.dir/flags.make
logger/client_logger/tests/CMakeFiles/mp_os_lggr_clnt_lggr_tests.dir/client_logger_tests.cpp.o: logger/client_logger/tests/client_logger_tests.cpp
logger/client_logger/tests/CMakeFiles/mp_os_lggr_clnt_lggr_tests.dir/client_logger_tests.cpp.o: logger/client_logger/tests/CMakeFiles/mp_os_lggr_clnt_lggr_tests.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/Users/ivan/desktop/c++/cpp_labs/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object logger/client_logger/tests/CMakeFiles/mp_os_lggr_clnt_lggr_tests.dir/client_logger_tests.cpp.o"
	cd /Users/ivan/desktop/c++/cpp_labs/logger/client_logger/tests && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT logger/client_logger/tests/CMakeFiles/mp_os_lggr_clnt_lggr_tests.dir/client_logger_tests.cpp.o -MF CMakeFiles/mp_os_lggr_clnt_lggr_tests.dir/client_logger_tests.cpp.o.d -o CMakeFiles/mp_os_lggr_clnt_lggr_tests.dir/client_logger_tests.cpp.o -c /Users/ivan/desktop/c++/cpp_labs/logger/client_logger/tests/client_logger_tests.cpp

logger/client_logger/tests/CMakeFiles/mp_os_lggr_clnt_lggr_tests.dir/client_logger_tests.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/mp_os_lggr_clnt_lggr_tests.dir/client_logger_tests.cpp.i"
	cd /Users/ivan/desktop/c++/cpp_labs/logger/client_logger/tests && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/ivan/desktop/c++/cpp_labs/logger/client_logger/tests/client_logger_tests.cpp > CMakeFiles/mp_os_lggr_clnt_lggr_tests.dir/client_logger_tests.cpp.i

logger/client_logger/tests/CMakeFiles/mp_os_lggr_clnt_lggr_tests.dir/client_logger_tests.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/mp_os_lggr_clnt_lggr_tests.dir/client_logger_tests.cpp.s"
	cd /Users/ivan/desktop/c++/cpp_labs/logger/client_logger/tests && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/ivan/desktop/c++/cpp_labs/logger/client_logger/tests/client_logger_tests.cpp -o CMakeFiles/mp_os_lggr_clnt_lggr_tests.dir/client_logger_tests.cpp.s

# Object files for target mp_os_lggr_clnt_lggr_tests
mp_os_lggr_clnt_lggr_tests_OBJECTS = \
"CMakeFiles/mp_os_lggr_clnt_lggr_tests.dir/client_logger_tests.cpp.o"

# External object files for target mp_os_lggr_clnt_lggr_tests
mp_os_lggr_clnt_lggr_tests_EXTERNAL_OBJECTS =

logger/client_logger/tests/mp_os_lggr_clnt_lggr_tests-1.0: logger/client_logger/tests/CMakeFiles/mp_os_lggr_clnt_lggr_tests.dir/client_logger_tests.cpp.o
logger/client_logger/tests/mp_os_lggr_clnt_lggr_tests-1.0: logger/client_logger/tests/CMakeFiles/mp_os_lggr_clnt_lggr_tests.dir/build.make
logger/client_logger/tests/mp_os_lggr_clnt_lggr_tests-1.0: lib/libgtest_main.a
logger/client_logger/tests/mp_os_lggr_clnt_lggr_tests-1.0: common/libmp_os_cmmn.a
logger/client_logger/tests/mp_os_lggr_clnt_lggr_tests-1.0: logger/logger/libmp_os_lggr_lggr.a
logger/client_logger/tests/mp_os_lggr_clnt_lggr_tests-1.0: logger/client_logger/libmp_os_lggr_clnt_lggr.a
logger/client_logger/tests/mp_os_lggr_clnt_lggr_tests-1.0: lib/libgtest.a
logger/client_logger/tests/mp_os_lggr_clnt_lggr_tests-1.0: common/libmp_os_cmmn.a
logger/client_logger/tests/mp_os_lggr_clnt_lggr_tests-1.0: logger/logger/libmp_os_lggr_lggr.a
logger/client_logger/tests/mp_os_lggr_clnt_lggr_tests-1.0: logger/client_logger/tests/CMakeFiles/mp_os_lggr_clnt_lggr_tests.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/Users/ivan/desktop/c++/cpp_labs/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable mp_os_lggr_clnt_lggr_tests"
	cd /Users/ivan/desktop/c++/cpp_labs/logger/client_logger/tests && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/mp_os_lggr_clnt_lggr_tests.dir/link.txt --verbose=$(VERBOSE)
	cd /Users/ivan/desktop/c++/cpp_labs/logger/client_logger/tests && $(CMAKE_COMMAND) -E cmake_symlink_executable mp_os_lggr_clnt_lggr_tests-1.0 mp_os_lggr_clnt_lggr_tests

logger/client_logger/tests/mp_os_lggr_clnt_lggr_tests: logger/client_logger/tests/mp_os_lggr_clnt_lggr_tests-1.0

# Rule to build all files generated by this target.
logger/client_logger/tests/CMakeFiles/mp_os_lggr_clnt_lggr_tests.dir/build: logger/client_logger/tests/mp_os_lggr_clnt_lggr_tests
.PHONY : logger/client_logger/tests/CMakeFiles/mp_os_lggr_clnt_lggr_tests.dir/build

logger/client_logger/tests/CMakeFiles/mp_os_lggr_clnt_lggr_tests.dir/clean:
	cd /Users/ivan/desktop/c++/cpp_labs/logger/client_logger/tests && $(CMAKE_COMMAND) -P CMakeFiles/mp_os_lggr_clnt_lggr_tests.dir/cmake_clean.cmake
.PHONY : logger/client_logger/tests/CMakeFiles/mp_os_lggr_clnt_lggr_tests.dir/clean

logger/client_logger/tests/CMakeFiles/mp_os_lggr_clnt_lggr_tests.dir/depend:
	cd /Users/ivan/desktop/c++/cpp_labs && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/ivan/desktop/c++/cpp_labs /Users/ivan/desktop/c++/cpp_labs/logger/client_logger/tests /Users/ivan/desktop/c++/cpp_labs /Users/ivan/desktop/c++/cpp_labs/logger/client_logger/tests /Users/ivan/desktop/c++/cpp_labs/logger/client_logger/tests/CMakeFiles/mp_os_lggr_clnt_lggr_tests.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : logger/client_logger/tests/CMakeFiles/mp_os_lggr_clnt_lggr_tests.dir/depend

