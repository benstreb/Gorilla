cmake_minimum_required (VERSION 2.6)
project (hw5)


# all the .cpp files that make up this project
add_executable(go
  main.cpp
  edge.cpp
  glCanvas.cpp
  camera.cpp
  mesh.cpp
  render.cpp
  matrix.cpp
  load_shaders.cpp
  boundingbox.cpp
  GoBoard.cpp
  ai_random.cpp
  ai_monte_carlo.cpp
  argparser.h
  camera.h
  glCanvas.h
  matrix.h
  triangle.h
  vertex.h
  boundingbox.h
  edge.h
  hash.h
  mesh.h
  vectors.h
  vbo_structs.h
  GoBoard.h
  ai.h
  ai_random.h
  ai_monte_carlo.h
)

# platform specific compiler flags to output all compiler warnings
if (UNIX)
  if (${CMAKE_SYSTEM_NAME} STREQUAL "FreeBSD")
    set_target_properties (go PROPERTIES COMPILE_FLAGS "-g -Wall -pedantic -DFreeBSD")
  else()
    set_target_properties (go PROPERTIES COMPILE_FLAGS "-g -Wall -pedantic -std=c++0x")
  endif()
endif()

if (APPLE)
set_target_properties (go PROPERTIES COMPILE_FLAGS "-g -Wall -pedantic") 
# -m32")
endif()

if (WIN32)
set_target_properties (go PROPERTIES COMPILE_FLAGS "/W4")
endif()



# a work-around function to handle a list of libraries that include a
#  NOTFOUND library
function (add_lib_list target liblist)
  foreach (lib ${liblist})
    if (lib)
      target_link_libraries(${target} "${lib}")
    else()
      message(STATUS "WARNING: missing library: ${lib}")
    endif()
  endforeach()
endfunction()



# search for the GL & GLUT & GLEW libraries

find_package(GLUT)
if (NOT GLUT_FOUND)
  message(FATAL_ERROR "Cannot find GLUT library")
endif()
message(STATUS "Found GLUT at \"${GLUT_LIBRARIES}\"")

find_package(OpenGL)
if (NOT OPENGL_FOUND)
   message(FATAL_ERROR "Cannot find OpenGL library")
endif()
message(STATUS "Found OpenGL at \"${OPENGL_LIBRARIES}\"")

add_lib_list(go "${OPENGL_LIBRARIES}")
add_lib_list(go "${GLUT_LIBRARIES}")

if (WIN32)
  find_library(GLEW_LIBRARIES glew32 HINT "lib")
  if (NOT GLEW_LIBRARIES)
    message(FATAL_ERROR "Cannot find GLEW library")
  endif()
  message(STATUS "Found GLEW at \"${GLEW_LIBRARIES}\"")
  add_lib_list(go "${GLEW_LIBRARIES}")
endif()

#include_directories(".")
