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
CMAKE_COMMAND = /home/cc/clion/clion-2020.1.3/bin/cmake/linux/bin/cmake

# The command to remove a file.
RM = /home/cc/clion/clion-2020.1.3/bin/cmake/linux/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/cc/DAMER1.0/DAMER

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/cc/DAMER1.0/DAMER/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/PDNet_checker.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/PDNet_checker.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/PDNet_checker.dir/flags.make

CMakeFiles/PDNet_checker.dir/main.cpp.o: CMakeFiles/PDNet_checker.dir/flags.make
CMakeFiles/PDNet_checker.dir/main.cpp.o: ../main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/cc/DAMER1.0/DAMER/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/PDNet_checker.dir/main.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/PDNet_checker.dir/main.cpp.o -c /home/cc/DAMER1.0/DAMER/main.cpp

CMakeFiles/PDNet_checker.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/PDNet_checker.dir/main.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/cc/DAMER1.0/DAMER/main.cpp > CMakeFiles/PDNet_checker.dir/main.cpp.i

CMakeFiles/PDNet_checker.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/PDNet_checker.dir/main.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/cc/DAMER1.0/DAMER/main.cpp -o CMakeFiles/PDNet_checker.dir/main.cpp.s

CMakeFiles/PDNet_checker.dir/AST.cpp.o: CMakeFiles/PDNet_checker.dir/flags.make
CMakeFiles/PDNet_checker.dir/AST.cpp.o: ../AST.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/cc/DAMER1.0/DAMER/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/PDNet_checker.dir/AST.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/PDNet_checker.dir/AST.cpp.o -c /home/cc/DAMER1.0/DAMER/AST.cpp

CMakeFiles/PDNet_checker.dir/AST.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/PDNet_checker.dir/AST.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/cc/DAMER1.0/DAMER/AST.cpp > CMakeFiles/PDNet_checker.dir/AST.cpp.i

CMakeFiles/PDNet_checker.dir/AST.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/PDNet_checker.dir/AST.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/cc/DAMER1.0/DAMER/AST.cpp -o CMakeFiles/PDNet_checker.dir/AST.cpp.s

CMakeFiles/PDNet_checker.dir/y.tab.cpp.o: CMakeFiles/PDNet_checker.dir/flags.make
CMakeFiles/PDNet_checker.dir/y.tab.cpp.o: ../y.tab.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/cc/DAMER1.0/DAMER/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/PDNet_checker.dir/y.tab.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/PDNet_checker.dir/y.tab.cpp.o -c /home/cc/DAMER1.0/DAMER/y.tab.cpp

CMakeFiles/PDNet_checker.dir/y.tab.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/PDNet_checker.dir/y.tab.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/cc/DAMER1.0/DAMER/y.tab.cpp > CMakeFiles/PDNet_checker.dir/y.tab.cpp.i

CMakeFiles/PDNet_checker.dir/y.tab.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/PDNet_checker.dir/y.tab.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/cc/DAMER1.0/DAMER/y.tab.cpp -o CMakeFiles/PDNet_checker.dir/y.tab.cpp.s

CMakeFiles/PDNet_checker.dir/lex.y.cpp.o: CMakeFiles/PDNet_checker.dir/flags.make
CMakeFiles/PDNet_checker.dir/lex.y.cpp.o: ../lex.y.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/cc/DAMER1.0/DAMER/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object CMakeFiles/PDNet_checker.dir/lex.y.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/PDNet_checker.dir/lex.y.cpp.o -c /home/cc/DAMER1.0/DAMER/lex.y.cpp

CMakeFiles/PDNet_checker.dir/lex.y.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/PDNet_checker.dir/lex.y.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/cc/DAMER1.0/DAMER/lex.y.cpp > CMakeFiles/PDNet_checker.dir/lex.y.cpp.i

CMakeFiles/PDNet_checker.dir/lex.y.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/PDNet_checker.dir/lex.y.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/cc/DAMER1.0/DAMER/lex.y.cpp -o CMakeFiles/PDNet_checker.dir/lex.y.cpp.s

CMakeFiles/PDNet_checker.dir/pre_process.cpp.o: CMakeFiles/PDNet_checker.dir/flags.make
CMakeFiles/PDNet_checker.dir/pre_process.cpp.o: ../pre_process.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/cc/DAMER1.0/DAMER/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building CXX object CMakeFiles/PDNet_checker.dir/pre_process.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/PDNet_checker.dir/pre_process.cpp.o -c /home/cc/DAMER1.0/DAMER/pre_process.cpp

CMakeFiles/PDNet_checker.dir/pre_process.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/PDNet_checker.dir/pre_process.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/cc/DAMER1.0/DAMER/pre_process.cpp > CMakeFiles/PDNet_checker.dir/pre_process.cpp.i

CMakeFiles/PDNet_checker.dir/pre_process.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/PDNet_checker.dir/pre_process.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/cc/DAMER1.0/DAMER/pre_process.cpp -o CMakeFiles/PDNet_checker.dir/pre_process.cpp.s

CMakeFiles/PDNet_checker.dir/base.cpp.o: CMakeFiles/PDNet_checker.dir/flags.make
CMakeFiles/PDNet_checker.dir/base.cpp.o: ../base.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/cc/DAMER1.0/DAMER/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Building CXX object CMakeFiles/PDNet_checker.dir/base.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/PDNet_checker.dir/base.cpp.o -c /home/cc/DAMER1.0/DAMER/base.cpp

CMakeFiles/PDNet_checker.dir/base.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/PDNet_checker.dir/base.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/cc/DAMER1.0/DAMER/base.cpp > CMakeFiles/PDNet_checker.dir/base.cpp.i

CMakeFiles/PDNet_checker.dir/base.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/PDNet_checker.dir/base.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/cc/DAMER1.0/DAMER/base.cpp -o CMakeFiles/PDNet_checker.dir/base.cpp.s

CMakeFiles/PDNet_checker.dir/cpn.cpp.o: CMakeFiles/PDNet_checker.dir/flags.make
CMakeFiles/PDNet_checker.dir/cpn.cpp.o: ../cpn.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/cc/DAMER1.0/DAMER/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Building CXX object CMakeFiles/PDNet_checker.dir/cpn.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/PDNet_checker.dir/cpn.cpp.o -c /home/cc/DAMER1.0/DAMER/cpn.cpp

CMakeFiles/PDNet_checker.dir/cpn.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/PDNet_checker.dir/cpn.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/cc/DAMER1.0/DAMER/cpn.cpp > CMakeFiles/PDNet_checker.dir/cpn.cpp.i

CMakeFiles/PDNet_checker.dir/cpn.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/PDNet_checker.dir/cpn.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/cc/DAMER1.0/DAMER/cpn.cpp -o CMakeFiles/PDNet_checker.dir/cpn.cpp.s

CMakeFiles/PDNet_checker.dir/cpn_rg.cpp.o: CMakeFiles/PDNet_checker.dir/flags.make
CMakeFiles/PDNet_checker.dir/cpn_rg.cpp.o: ../cpn_rg.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/cc/DAMER1.0/DAMER/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_8) "Building CXX object CMakeFiles/PDNet_checker.dir/cpn_rg.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/PDNet_checker.dir/cpn_rg.cpp.o -c /home/cc/DAMER1.0/DAMER/cpn_rg.cpp

CMakeFiles/PDNet_checker.dir/cpn_rg.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/PDNet_checker.dir/cpn_rg.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/cc/DAMER1.0/DAMER/cpn_rg.cpp > CMakeFiles/PDNet_checker.dir/cpn_rg.cpp.i

CMakeFiles/PDNet_checker.dir/cpn_rg.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/PDNet_checker.dir/cpn_rg.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/cc/DAMER1.0/DAMER/cpn_rg.cpp -o CMakeFiles/PDNet_checker.dir/cpn_rg.cpp.s

CMakeFiles/PDNet_checker.dir/slice.cpp.o: CMakeFiles/PDNet_checker.dir/flags.make
CMakeFiles/PDNet_checker.dir/slice.cpp.o: ../slice.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/cc/DAMER1.0/DAMER/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_9) "Building CXX object CMakeFiles/PDNet_checker.dir/slice.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/PDNet_checker.dir/slice.cpp.o -c /home/cc/DAMER1.0/DAMER/slice.cpp

CMakeFiles/PDNet_checker.dir/slice.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/PDNet_checker.dir/slice.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/cc/DAMER1.0/DAMER/slice.cpp > CMakeFiles/PDNet_checker.dir/slice.cpp.i

CMakeFiles/PDNet_checker.dir/slice.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/PDNet_checker.dir/slice.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/cc/DAMER1.0/DAMER/slice.cpp -o CMakeFiles/PDNet_checker.dir/slice.cpp.s

CMakeFiles/PDNet_checker.dir/product.cpp.o: CMakeFiles/PDNet_checker.dir/flags.make
CMakeFiles/PDNet_checker.dir/product.cpp.o: ../product.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/cc/DAMER1.0/DAMER/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_10) "Building CXX object CMakeFiles/PDNet_checker.dir/product.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/PDNet_checker.dir/product.cpp.o -c /home/cc/DAMER1.0/DAMER/product.cpp

CMakeFiles/PDNet_checker.dir/product.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/PDNet_checker.dir/product.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/cc/DAMER1.0/DAMER/product.cpp > CMakeFiles/PDNet_checker.dir/product.cpp.i

CMakeFiles/PDNet_checker.dir/product.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/PDNet_checker.dir/product.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/cc/DAMER1.0/DAMER/product.cpp -o CMakeFiles/PDNet_checker.dir/product.cpp.s

CMakeFiles/PDNet_checker.dir/BA/tinystr.cpp.o: CMakeFiles/PDNet_checker.dir/flags.make
CMakeFiles/PDNet_checker.dir/BA/tinystr.cpp.o: ../BA/tinystr.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/cc/DAMER1.0/DAMER/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_11) "Building CXX object CMakeFiles/PDNet_checker.dir/BA/tinystr.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/PDNet_checker.dir/BA/tinystr.cpp.o -c /home/cc/DAMER1.0/DAMER/BA/tinystr.cpp

CMakeFiles/PDNet_checker.dir/BA/tinystr.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/PDNet_checker.dir/BA/tinystr.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/cc/DAMER1.0/DAMER/BA/tinystr.cpp > CMakeFiles/PDNet_checker.dir/BA/tinystr.cpp.i

CMakeFiles/PDNet_checker.dir/BA/tinystr.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/PDNet_checker.dir/BA/tinystr.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/cc/DAMER1.0/DAMER/BA/tinystr.cpp -o CMakeFiles/PDNet_checker.dir/BA/tinystr.cpp.s

CMakeFiles/PDNet_checker.dir/BA/tinyxml.cpp.o: CMakeFiles/PDNet_checker.dir/flags.make
CMakeFiles/PDNet_checker.dir/BA/tinyxml.cpp.o: ../BA/tinyxml.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/cc/DAMER1.0/DAMER/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_12) "Building CXX object CMakeFiles/PDNet_checker.dir/BA/tinyxml.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/PDNet_checker.dir/BA/tinyxml.cpp.o -c /home/cc/DAMER1.0/DAMER/BA/tinyxml.cpp

CMakeFiles/PDNet_checker.dir/BA/tinyxml.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/PDNet_checker.dir/BA/tinyxml.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/cc/DAMER1.0/DAMER/BA/tinyxml.cpp > CMakeFiles/PDNet_checker.dir/BA/tinyxml.cpp.i

CMakeFiles/PDNet_checker.dir/BA/tinyxml.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/PDNet_checker.dir/BA/tinyxml.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/cc/DAMER1.0/DAMER/BA/tinyxml.cpp -o CMakeFiles/PDNet_checker.dir/BA/tinyxml.cpp.s

CMakeFiles/PDNet_checker.dir/BA/tinyxmlerror.cpp.o: CMakeFiles/PDNet_checker.dir/flags.make
CMakeFiles/PDNet_checker.dir/BA/tinyxmlerror.cpp.o: ../BA/tinyxmlerror.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/cc/DAMER1.0/DAMER/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_13) "Building CXX object CMakeFiles/PDNet_checker.dir/BA/tinyxmlerror.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/PDNet_checker.dir/BA/tinyxmlerror.cpp.o -c /home/cc/DAMER1.0/DAMER/BA/tinyxmlerror.cpp

CMakeFiles/PDNet_checker.dir/BA/tinyxmlerror.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/PDNet_checker.dir/BA/tinyxmlerror.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/cc/DAMER1.0/DAMER/BA/tinyxmlerror.cpp > CMakeFiles/PDNet_checker.dir/BA/tinyxmlerror.cpp.i

CMakeFiles/PDNet_checker.dir/BA/tinyxmlerror.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/PDNet_checker.dir/BA/tinyxmlerror.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/cc/DAMER1.0/DAMER/BA/tinyxmlerror.cpp -o CMakeFiles/PDNet_checker.dir/BA/tinyxmlerror.cpp.s

CMakeFiles/PDNet_checker.dir/BA/tinyxmlparser.cpp.o: CMakeFiles/PDNet_checker.dir/flags.make
CMakeFiles/PDNet_checker.dir/BA/tinyxmlparser.cpp.o: ../BA/tinyxmlparser.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/cc/DAMER1.0/DAMER/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_14) "Building CXX object CMakeFiles/PDNet_checker.dir/BA/tinyxmlparser.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/PDNet_checker.dir/BA/tinyxmlparser.cpp.o -c /home/cc/DAMER1.0/DAMER/BA/tinyxmlparser.cpp

CMakeFiles/PDNet_checker.dir/BA/tinyxmlparser.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/PDNet_checker.dir/BA/tinyxmlparser.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/cc/DAMER1.0/DAMER/BA/tinyxmlparser.cpp > CMakeFiles/PDNet_checker.dir/BA/tinyxmlparser.cpp.i

CMakeFiles/PDNet_checker.dir/BA/tinyxmlparser.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/PDNet_checker.dir/BA/tinyxmlparser.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/cc/DAMER1.0/DAMER/BA/tinyxmlparser.cpp -o CMakeFiles/PDNet_checker.dir/BA/tinyxmlparser.cpp.s

CMakeFiles/PDNet_checker.dir/BA/Syntax_Tree.cpp.o: CMakeFiles/PDNet_checker.dir/flags.make
CMakeFiles/PDNet_checker.dir/BA/Syntax_Tree.cpp.o: ../BA/Syntax_Tree.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/cc/DAMER1.0/DAMER/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_15) "Building CXX object CMakeFiles/PDNet_checker.dir/BA/Syntax_Tree.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/PDNet_checker.dir/BA/Syntax_Tree.cpp.o -c /home/cc/DAMER1.0/DAMER/BA/Syntax_Tree.cpp

CMakeFiles/PDNet_checker.dir/BA/Syntax_Tree.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/PDNet_checker.dir/BA/Syntax_Tree.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/cc/DAMER1.0/DAMER/BA/Syntax_Tree.cpp > CMakeFiles/PDNet_checker.dir/BA/Syntax_Tree.cpp.i

CMakeFiles/PDNet_checker.dir/BA/Syntax_Tree.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/PDNet_checker.dir/BA/Syntax_Tree.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/cc/DAMER1.0/DAMER/BA/Syntax_Tree.cpp -o CMakeFiles/PDNet_checker.dir/BA/Syntax_Tree.cpp.s

CMakeFiles/PDNet_checker.dir/BA/VWAA.cpp.o: CMakeFiles/PDNet_checker.dir/flags.make
CMakeFiles/PDNet_checker.dir/BA/VWAA.cpp.o: ../BA/VWAA.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/cc/DAMER1.0/DAMER/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_16) "Building CXX object CMakeFiles/PDNet_checker.dir/BA/VWAA.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/PDNet_checker.dir/BA/VWAA.cpp.o -c /home/cc/DAMER1.0/DAMER/BA/VWAA.cpp

CMakeFiles/PDNet_checker.dir/BA/VWAA.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/PDNet_checker.dir/BA/VWAA.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/cc/DAMER1.0/DAMER/BA/VWAA.cpp > CMakeFiles/PDNet_checker.dir/BA/VWAA.cpp.i

CMakeFiles/PDNet_checker.dir/BA/VWAA.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/PDNet_checker.dir/BA/VWAA.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/cc/DAMER1.0/DAMER/BA/VWAA.cpp -o CMakeFiles/PDNet_checker.dir/BA/VWAA.cpp.s

CMakeFiles/PDNet_checker.dir/BA/buchi.cpp.o: CMakeFiles/PDNet_checker.dir/flags.make
CMakeFiles/PDNet_checker.dir/BA/buchi.cpp.o: ../BA/buchi.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/cc/DAMER1.0/DAMER/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_17) "Building CXX object CMakeFiles/PDNet_checker.dir/BA/buchi.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/PDNet_checker.dir/BA/buchi.cpp.o -c /home/cc/DAMER1.0/DAMER/BA/buchi.cpp

CMakeFiles/PDNet_checker.dir/BA/buchi.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/PDNet_checker.dir/BA/buchi.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/cc/DAMER1.0/DAMER/BA/buchi.cpp > CMakeFiles/PDNet_checker.dir/BA/buchi.cpp.i

CMakeFiles/PDNet_checker.dir/BA/buchi.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/PDNet_checker.dir/BA/buchi.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/cc/DAMER1.0/DAMER/BA/buchi.cpp -o CMakeFiles/PDNet_checker.dir/BA/buchi.cpp.s

CMakeFiles/PDNet_checker.dir/criteria.cpp.o: CMakeFiles/PDNet_checker.dir/flags.make
CMakeFiles/PDNet_checker.dir/criteria.cpp.o: ../criteria.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/cc/DAMER1.0/DAMER/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_18) "Building CXX object CMakeFiles/PDNet_checker.dir/criteria.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/PDNet_checker.dir/criteria.cpp.o -c /home/cc/DAMER1.0/DAMER/criteria.cpp

CMakeFiles/PDNet_checker.dir/criteria.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/PDNet_checker.dir/criteria.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/cc/DAMER1.0/DAMER/criteria.cpp > CMakeFiles/PDNet_checker.dir/criteria.cpp.i

CMakeFiles/PDNet_checker.dir/criteria.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/PDNet_checker.dir/criteria.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/cc/DAMER1.0/DAMER/criteria.cpp -o CMakeFiles/PDNet_checker.dir/criteria.cpp.s

CMakeFiles/PDNet_checker.dir/v_table.cpp.o: CMakeFiles/PDNet_checker.dir/flags.make
CMakeFiles/PDNet_checker.dir/v_table.cpp.o: ../v_table.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/cc/DAMER1.0/DAMER/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_19) "Building CXX object CMakeFiles/PDNet_checker.dir/v_table.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/PDNet_checker.dir/v_table.cpp.o -c /home/cc/DAMER1.0/DAMER/v_table.cpp

CMakeFiles/PDNet_checker.dir/v_table.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/PDNet_checker.dir/v_table.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/cc/DAMER1.0/DAMER/v_table.cpp > CMakeFiles/PDNet_checker.dir/v_table.cpp.i

CMakeFiles/PDNet_checker.dir/v_table.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/PDNet_checker.dir/v_table.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/cc/DAMER1.0/DAMER/v_table.cpp -o CMakeFiles/PDNet_checker.dir/v_table.cpp.s

CMakeFiles/PDNet_checker.dir/pdnet_parseoption.cpp.o: CMakeFiles/PDNet_checker.dir/flags.make
CMakeFiles/PDNet_checker.dir/pdnet_parseoption.cpp.o: ../pdnet_parseoption.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/cc/DAMER1.0/DAMER/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_20) "Building CXX object CMakeFiles/PDNet_checker.dir/pdnet_parseoption.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/PDNet_checker.dir/pdnet_parseoption.cpp.o -c /home/cc/DAMER1.0/DAMER/pdnet_parseoption.cpp

CMakeFiles/PDNet_checker.dir/pdnet_parseoption.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/PDNet_checker.dir/pdnet_parseoption.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/cc/DAMER1.0/DAMER/pdnet_parseoption.cpp > CMakeFiles/PDNet_checker.dir/pdnet_parseoption.cpp.i

CMakeFiles/PDNet_checker.dir/pdnet_parseoption.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/PDNet_checker.dir/pdnet_parseoption.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/cc/DAMER1.0/DAMER/pdnet_parseoption.cpp -o CMakeFiles/PDNet_checker.dir/pdnet_parseoption.cpp.s

CMakeFiles/PDNet_checker.dir/expression_tree.cpp.o: CMakeFiles/PDNet_checker.dir/flags.make
CMakeFiles/PDNet_checker.dir/expression_tree.cpp.o: ../expression_tree.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/cc/DAMER1.0/DAMER/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_21) "Building CXX object CMakeFiles/PDNet_checker.dir/expression_tree.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/PDNet_checker.dir/expression_tree.cpp.o -c /home/cc/DAMER1.0/DAMER/expression_tree.cpp

CMakeFiles/PDNet_checker.dir/expression_tree.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/PDNet_checker.dir/expression_tree.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/cc/DAMER1.0/DAMER/expression_tree.cpp > CMakeFiles/PDNet_checker.dir/expression_tree.cpp.i

CMakeFiles/PDNet_checker.dir/expression_tree.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/PDNet_checker.dir/expression_tree.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/cc/DAMER1.0/DAMER/expression_tree.cpp -o CMakeFiles/PDNet_checker.dir/expression_tree.cpp.s

# Object files for target PDNet_checker
PDNet_checker_OBJECTS = \
"CMakeFiles/PDNet_checker.dir/main.cpp.o" \
"CMakeFiles/PDNet_checker.dir/AST.cpp.o" \
"CMakeFiles/PDNet_checker.dir/y.tab.cpp.o" \
"CMakeFiles/PDNet_checker.dir/lex.y.cpp.o" \
"CMakeFiles/PDNet_checker.dir/pre_process.cpp.o" \
"CMakeFiles/PDNet_checker.dir/base.cpp.o" \
"CMakeFiles/PDNet_checker.dir/cpn.cpp.o" \
"CMakeFiles/PDNet_checker.dir/cpn_rg.cpp.o" \
"CMakeFiles/PDNet_checker.dir/slice.cpp.o" \
"CMakeFiles/PDNet_checker.dir/product.cpp.o" \
"CMakeFiles/PDNet_checker.dir/BA/tinystr.cpp.o" \
"CMakeFiles/PDNet_checker.dir/BA/tinyxml.cpp.o" \
"CMakeFiles/PDNet_checker.dir/BA/tinyxmlerror.cpp.o" \
"CMakeFiles/PDNet_checker.dir/BA/tinyxmlparser.cpp.o" \
"CMakeFiles/PDNet_checker.dir/BA/Syntax_Tree.cpp.o" \
"CMakeFiles/PDNet_checker.dir/BA/VWAA.cpp.o" \
"CMakeFiles/PDNet_checker.dir/BA/buchi.cpp.o" \
"CMakeFiles/PDNet_checker.dir/criteria.cpp.o" \
"CMakeFiles/PDNet_checker.dir/v_table.cpp.o" \
"CMakeFiles/PDNet_checker.dir/pdnet_parseoption.cpp.o" \
"CMakeFiles/PDNet_checker.dir/expression_tree.cpp.o"

# External object files for target PDNet_checker
PDNet_checker_EXTERNAL_OBJECTS =

PDNet_checker: CMakeFiles/PDNet_checker.dir/main.cpp.o
PDNet_checker: CMakeFiles/PDNet_checker.dir/AST.cpp.o
PDNet_checker: CMakeFiles/PDNet_checker.dir/y.tab.cpp.o
PDNet_checker: CMakeFiles/PDNet_checker.dir/lex.y.cpp.o
PDNet_checker: CMakeFiles/PDNet_checker.dir/pre_process.cpp.o
PDNet_checker: CMakeFiles/PDNet_checker.dir/base.cpp.o
PDNet_checker: CMakeFiles/PDNet_checker.dir/cpn.cpp.o
PDNet_checker: CMakeFiles/PDNet_checker.dir/cpn_rg.cpp.o
PDNet_checker: CMakeFiles/PDNet_checker.dir/slice.cpp.o
PDNet_checker: CMakeFiles/PDNet_checker.dir/product.cpp.o
PDNet_checker: CMakeFiles/PDNet_checker.dir/BA/tinystr.cpp.o
PDNet_checker: CMakeFiles/PDNet_checker.dir/BA/tinyxml.cpp.o
PDNet_checker: CMakeFiles/PDNet_checker.dir/BA/tinyxmlerror.cpp.o
PDNet_checker: CMakeFiles/PDNet_checker.dir/BA/tinyxmlparser.cpp.o
PDNet_checker: CMakeFiles/PDNet_checker.dir/BA/Syntax_Tree.cpp.o
PDNet_checker: CMakeFiles/PDNet_checker.dir/BA/VWAA.cpp.o
PDNet_checker: CMakeFiles/PDNet_checker.dir/BA/buchi.cpp.o
PDNet_checker: CMakeFiles/PDNet_checker.dir/criteria.cpp.o
PDNet_checker: CMakeFiles/PDNet_checker.dir/v_table.cpp.o
PDNet_checker: CMakeFiles/PDNet_checker.dir/pdnet_parseoption.cpp.o
PDNet_checker: CMakeFiles/PDNet_checker.dir/expression_tree.cpp.o
PDNet_checker: CMakeFiles/PDNet_checker.dir/build.make
PDNet_checker: CMakeFiles/PDNet_checker.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/cc/DAMER1.0/DAMER/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_22) "Linking CXX executable PDNet_checker"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/PDNet_checker.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/PDNet_checker.dir/build: PDNet_checker

.PHONY : CMakeFiles/PDNet_checker.dir/build

CMakeFiles/PDNet_checker.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/PDNet_checker.dir/cmake_clean.cmake
.PHONY : CMakeFiles/PDNet_checker.dir/clean

CMakeFiles/PDNet_checker.dir/depend:
	cd /home/cc/DAMER1.0/DAMER/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/cc/DAMER1.0/DAMER /home/cc/DAMER1.0/DAMER /home/cc/DAMER1.0/DAMER/cmake-build-debug /home/cc/DAMER1.0/DAMER/cmake-build-debug /home/cc/DAMER1.0/DAMER/cmake-build-debug/CMakeFiles/PDNet_checker.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/PDNet_checker.dir/depend

