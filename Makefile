# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.8

# Default target executed when no arguments are given to make.
default_target: all
.PHONY : default_target

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

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/wrigha3/Gorilla

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/wrigha3/Gorilla

#=============================================================================
# Targets provided globally by CMake.

# Special rule for the target edit_cache
edit_cache:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "Running interactive CMake command-line interface..."
	/usr/bin/cmake -i .
.PHONY : edit_cache

# Special rule for the target edit_cache
edit_cache/fast: edit_cache
.PHONY : edit_cache/fast

# Special rule for the target rebuild_cache
rebuild_cache:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "Running CMake to regenerate build system..."
	/usr/bin/cmake -H$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR)
.PHONY : rebuild_cache

# Special rule for the target rebuild_cache
rebuild_cache/fast: rebuild_cache
.PHONY : rebuild_cache/fast

# The main all target
all: cmake_check_build_system
	$(CMAKE_COMMAND) -E cmake_progress_start /home/wrigha3/Gorilla/CMakeFiles /home/wrigha3/Gorilla/CMakeFiles/progress.marks
	$(MAKE) -f CMakeFiles/Makefile2 all
	$(CMAKE_COMMAND) -E cmake_progress_start /home/wrigha3/Gorilla/CMakeFiles 0
.PHONY : all

# The main clean target
clean:
	$(MAKE) -f CMakeFiles/Makefile2 clean
.PHONY : clean

# The main clean target
clean/fast: clean
.PHONY : clean/fast

# Prepare targets for installation.
preinstall: all
	$(MAKE) -f CMakeFiles/Makefile2 preinstall
.PHONY : preinstall

# Prepare targets for installation.
preinstall/fast:
	$(MAKE) -f CMakeFiles/Makefile2 preinstall
.PHONY : preinstall/fast

# clear depends
depend:
	$(CMAKE_COMMAND) -H$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 1
.PHONY : depend

#=============================================================================
# Target rules for targets named go

# Build rule for target.
go: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 go
.PHONY : go

# fast build rule for target.
go/fast:
	$(MAKE) -f CMakeFiles/go.dir/build.make CMakeFiles/go.dir/build
.PHONY : go/fast

GoBoard.o: GoBoard.cpp.o
.PHONY : GoBoard.o

# target to build an object file
GoBoard.cpp.o:
	$(MAKE) -f CMakeFiles/go.dir/build.make CMakeFiles/go.dir/GoBoard.cpp.o
.PHONY : GoBoard.cpp.o

GoBoard.i: GoBoard.cpp.i
.PHONY : GoBoard.i

# target to preprocess a source file
GoBoard.cpp.i:
	$(MAKE) -f CMakeFiles/go.dir/build.make CMakeFiles/go.dir/GoBoard.cpp.i
.PHONY : GoBoard.cpp.i

GoBoard.s: GoBoard.cpp.s
.PHONY : GoBoard.s

# target to generate assembly for a file
GoBoard.cpp.s:
	$(MAKE) -f CMakeFiles/go.dir/build.make CMakeFiles/go.dir/GoBoard.cpp.s
.PHONY : GoBoard.cpp.s

ai_monte_carlo.o: ai_monte_carlo.cpp.o
.PHONY : ai_monte_carlo.o

# target to build an object file
ai_monte_carlo.cpp.o:
	$(MAKE) -f CMakeFiles/go.dir/build.make CMakeFiles/go.dir/ai_monte_carlo.cpp.o
.PHONY : ai_monte_carlo.cpp.o

ai_monte_carlo.i: ai_monte_carlo.cpp.i
.PHONY : ai_monte_carlo.i

# target to preprocess a source file
ai_monte_carlo.cpp.i:
	$(MAKE) -f CMakeFiles/go.dir/build.make CMakeFiles/go.dir/ai_monte_carlo.cpp.i
.PHONY : ai_monte_carlo.cpp.i

ai_monte_carlo.s: ai_monte_carlo.cpp.s
.PHONY : ai_monte_carlo.s

# target to generate assembly for a file
ai_monte_carlo.cpp.s:
	$(MAKE) -f CMakeFiles/go.dir/build.make CMakeFiles/go.dir/ai_monte_carlo.cpp.s
.PHONY : ai_monte_carlo.cpp.s

ai_random.o: ai_random.cpp.o
.PHONY : ai_random.o

# target to build an object file
ai_random.cpp.o:
	$(MAKE) -f CMakeFiles/go.dir/build.make CMakeFiles/go.dir/ai_random.cpp.o
.PHONY : ai_random.cpp.o

ai_random.i: ai_random.cpp.i
.PHONY : ai_random.i

# target to preprocess a source file
ai_random.cpp.i:
	$(MAKE) -f CMakeFiles/go.dir/build.make CMakeFiles/go.dir/ai_random.cpp.i
.PHONY : ai_random.cpp.i

ai_random.s: ai_random.cpp.s
.PHONY : ai_random.s

# target to generate assembly for a file
ai_random.cpp.s:
	$(MAKE) -f CMakeFiles/go.dir/build.make CMakeFiles/go.dir/ai_random.cpp.s
.PHONY : ai_random.cpp.s

boundingbox.o: boundingbox.cpp.o
.PHONY : boundingbox.o

# target to build an object file
boundingbox.cpp.o:
	$(MAKE) -f CMakeFiles/go.dir/build.make CMakeFiles/go.dir/boundingbox.cpp.o
.PHONY : boundingbox.cpp.o

boundingbox.i: boundingbox.cpp.i
.PHONY : boundingbox.i

# target to preprocess a source file
boundingbox.cpp.i:
	$(MAKE) -f CMakeFiles/go.dir/build.make CMakeFiles/go.dir/boundingbox.cpp.i
.PHONY : boundingbox.cpp.i

boundingbox.s: boundingbox.cpp.s
.PHONY : boundingbox.s

# target to generate assembly for a file
boundingbox.cpp.s:
	$(MAKE) -f CMakeFiles/go.dir/build.make CMakeFiles/go.dir/boundingbox.cpp.s
.PHONY : boundingbox.cpp.s

camera.o: camera.cpp.o
.PHONY : camera.o

# target to build an object file
camera.cpp.o:
	$(MAKE) -f CMakeFiles/go.dir/build.make CMakeFiles/go.dir/camera.cpp.o
.PHONY : camera.cpp.o

camera.i: camera.cpp.i
.PHONY : camera.i

# target to preprocess a source file
camera.cpp.i:
	$(MAKE) -f CMakeFiles/go.dir/build.make CMakeFiles/go.dir/camera.cpp.i
.PHONY : camera.cpp.i

camera.s: camera.cpp.s
.PHONY : camera.s

# target to generate assembly for a file
camera.cpp.s:
	$(MAKE) -f CMakeFiles/go.dir/build.make CMakeFiles/go.dir/camera.cpp.s
.PHONY : camera.cpp.s

edge.o: edge.cpp.o
.PHONY : edge.o

# target to build an object file
edge.cpp.o:
	$(MAKE) -f CMakeFiles/go.dir/build.make CMakeFiles/go.dir/edge.cpp.o
.PHONY : edge.cpp.o

edge.i: edge.cpp.i
.PHONY : edge.i

# target to preprocess a source file
edge.cpp.i:
	$(MAKE) -f CMakeFiles/go.dir/build.make CMakeFiles/go.dir/edge.cpp.i
.PHONY : edge.cpp.i

edge.s: edge.cpp.s
.PHONY : edge.s

# target to generate assembly for a file
edge.cpp.s:
	$(MAKE) -f CMakeFiles/go.dir/build.make CMakeFiles/go.dir/edge.cpp.s
.PHONY : edge.cpp.s

glCanvas.o: glCanvas.cpp.o
.PHONY : glCanvas.o

# target to build an object file
glCanvas.cpp.o:
	$(MAKE) -f CMakeFiles/go.dir/build.make CMakeFiles/go.dir/glCanvas.cpp.o
.PHONY : glCanvas.cpp.o

glCanvas.i: glCanvas.cpp.i
.PHONY : glCanvas.i

# target to preprocess a source file
glCanvas.cpp.i:
	$(MAKE) -f CMakeFiles/go.dir/build.make CMakeFiles/go.dir/glCanvas.cpp.i
.PHONY : glCanvas.cpp.i

glCanvas.s: glCanvas.cpp.s
.PHONY : glCanvas.s

# target to generate assembly for a file
glCanvas.cpp.s:
	$(MAKE) -f CMakeFiles/go.dir/build.make CMakeFiles/go.dir/glCanvas.cpp.s
.PHONY : glCanvas.cpp.s

load_shaders.o: load_shaders.cpp.o
.PHONY : load_shaders.o

# target to build an object file
load_shaders.cpp.o:
	$(MAKE) -f CMakeFiles/go.dir/build.make CMakeFiles/go.dir/load_shaders.cpp.o
.PHONY : load_shaders.cpp.o

load_shaders.i: load_shaders.cpp.i
.PHONY : load_shaders.i

# target to preprocess a source file
load_shaders.cpp.i:
	$(MAKE) -f CMakeFiles/go.dir/build.make CMakeFiles/go.dir/load_shaders.cpp.i
.PHONY : load_shaders.cpp.i

load_shaders.s: load_shaders.cpp.s
.PHONY : load_shaders.s

# target to generate assembly for a file
load_shaders.cpp.s:
	$(MAKE) -f CMakeFiles/go.dir/build.make CMakeFiles/go.dir/load_shaders.cpp.s
.PHONY : load_shaders.cpp.s

main.o: main.cpp.o
.PHONY : main.o

# target to build an object file
main.cpp.o:
	$(MAKE) -f CMakeFiles/go.dir/build.make CMakeFiles/go.dir/main.cpp.o
.PHONY : main.cpp.o

main.i: main.cpp.i
.PHONY : main.i

# target to preprocess a source file
main.cpp.i:
	$(MAKE) -f CMakeFiles/go.dir/build.make CMakeFiles/go.dir/main.cpp.i
.PHONY : main.cpp.i

main.s: main.cpp.s
.PHONY : main.s

# target to generate assembly for a file
main.cpp.s:
	$(MAKE) -f CMakeFiles/go.dir/build.make CMakeFiles/go.dir/main.cpp.s
.PHONY : main.cpp.s

matrix.o: matrix.cpp.o
.PHONY : matrix.o

# target to build an object file
matrix.cpp.o:
	$(MAKE) -f CMakeFiles/go.dir/build.make CMakeFiles/go.dir/matrix.cpp.o
.PHONY : matrix.cpp.o

matrix.i: matrix.cpp.i
.PHONY : matrix.i

# target to preprocess a source file
matrix.cpp.i:
	$(MAKE) -f CMakeFiles/go.dir/build.make CMakeFiles/go.dir/matrix.cpp.i
.PHONY : matrix.cpp.i

matrix.s: matrix.cpp.s
.PHONY : matrix.s

# target to generate assembly for a file
matrix.cpp.s:
	$(MAKE) -f CMakeFiles/go.dir/build.make CMakeFiles/go.dir/matrix.cpp.s
.PHONY : matrix.cpp.s

mesh.o: mesh.cpp.o
.PHONY : mesh.o

# target to build an object file
mesh.cpp.o:
	$(MAKE) -f CMakeFiles/go.dir/build.make CMakeFiles/go.dir/mesh.cpp.o
.PHONY : mesh.cpp.o

mesh.i: mesh.cpp.i
.PHONY : mesh.i

# target to preprocess a source file
mesh.cpp.i:
	$(MAKE) -f CMakeFiles/go.dir/build.make CMakeFiles/go.dir/mesh.cpp.i
.PHONY : mesh.cpp.i

mesh.s: mesh.cpp.s
.PHONY : mesh.s

# target to generate assembly for a file
mesh.cpp.s:
	$(MAKE) -f CMakeFiles/go.dir/build.make CMakeFiles/go.dir/mesh.cpp.s
.PHONY : mesh.cpp.s

render.o: render.cpp.o
.PHONY : render.o

# target to build an object file
render.cpp.o:
	$(MAKE) -f CMakeFiles/go.dir/build.make CMakeFiles/go.dir/render.cpp.o
.PHONY : render.cpp.o

render.i: render.cpp.i
.PHONY : render.i

# target to preprocess a source file
render.cpp.i:
	$(MAKE) -f CMakeFiles/go.dir/build.make CMakeFiles/go.dir/render.cpp.i
.PHONY : render.cpp.i

render.s: render.cpp.s
.PHONY : render.s

# target to generate assembly for a file
render.cpp.s:
	$(MAKE) -f CMakeFiles/go.dir/build.make CMakeFiles/go.dir/render.cpp.s
.PHONY : render.cpp.s

# Help Target
help:
	@echo "The following are some of the valid targets for this Makefile:"
	@echo "... all (the default if no target is provided)"
	@echo "... clean"
	@echo "... depend"
	@echo "... edit_cache"
	@echo "... go"
	@echo "... rebuild_cache"
	@echo "... GoBoard.o"
	@echo "... GoBoard.i"
	@echo "... GoBoard.s"
	@echo "... ai_monte_carlo.o"
	@echo "... ai_monte_carlo.i"
	@echo "... ai_monte_carlo.s"
	@echo "... ai_random.o"
	@echo "... ai_random.i"
	@echo "... ai_random.s"
	@echo "... boundingbox.o"
	@echo "... boundingbox.i"
	@echo "... boundingbox.s"
	@echo "... camera.o"
	@echo "... camera.i"
	@echo "... camera.s"
	@echo "... edge.o"
	@echo "... edge.i"
	@echo "... edge.s"
	@echo "... glCanvas.o"
	@echo "... glCanvas.i"
	@echo "... glCanvas.s"
	@echo "... load_shaders.o"
	@echo "... load_shaders.i"
	@echo "... load_shaders.s"
	@echo "... main.o"
	@echo "... main.i"
	@echo "... main.s"
	@echo "... matrix.o"
	@echo "... matrix.i"
	@echo "... matrix.s"
	@echo "... mesh.o"
	@echo "... mesh.i"
	@echo "... mesh.s"
	@echo "... render.o"
	@echo "... render.i"
	@echo "... render.s"
.PHONY : help



#=============================================================================
# Special targets to cleanup operation of make.

# Special rule to run CMake to check the build system integrity.
# No rule that depends on this can have commands that come from listfiles
# because they might be regenerated.
cmake_check_build_system:
	$(CMAKE_COMMAND) -H$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 0
.PHONY : cmake_check_build_system

