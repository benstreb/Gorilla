#ifndef __ARG_PARSER_H__
#define __ARG_PARSER_H__

#include <cstdio>
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <string>

#include "ai.h"
#include "ai_random.h"

class ArgParser {

public:

  ArgParser() { DefaultValues(); }

  ArgParser(int argc, char *argv[]) {
    DefaultValues();
    // parse the command line arguments
    for (int i = 1; i < argc; i++) {
      if (!strcmp(argv[i],"-input") ||
	  !strcmp(argv[i],"-i")) {
	i++; assert (i < argc);
	input_file = std::string(argv[i]);
      } else if (!strcmp(argv[i],"-vertex") ||
		 !strcmp(argv[i],"-v")) {
	i++; assert (i < argc); 
	vertex_shader_filename = std::string(argv[i]);
      } else if (!strcmp(argv[i],"-fragment") ||
		 !strcmp(argv[i],"-f")) {
	i++; assert (i < argc); 
	fragment_shader_filename = std::string(argv[i]);
      } else if (!strcmp(argv[i],"-size")) {
	i++; assert (i < argc); 
	width = height = atoi(argv[i]);
      } else if (!strcmp(argv[i],"-2lights")) {
	two_lights = true;;
      } else if (!strcmp(argv[i],"-ai_random")) {
		theAI = new AI_Random(-1);
		using_ai = true;
      } else {
	printf ("whoops error with command line argument %d: '%s'\n",i,argv[i]);
	assert(0);
      }
    }
  }

  void DefaultValues() {
    input_file = "";
    vertex_shader_filename = "";
    fragment_shader_filename = "";
    width = 1400;
    height = 700;
    mirror = false;
    shadow = false;
    geometry = true;
    reflected_geometry = false;
    bounding_box = false;
    silhouette_edges = false;
    shadow_polygons = false;
    gouraud_normals = false;
    glsl_enabled = false;
    glsl_initialized = false;
    two_lights = false;
    timer = 0.0;
    using_ai = false;
  }

  // ==============
  // REPRESENTATION
  // all public! (no accessors)

  std::string input_file;
  std::string fragment_shader_filename;
  std::string vertex_shader_filename;
  int width;
  int height;
  bool board_control;
  bool shadow;
  bool mirror;
  bool geometry;
  bool reflected_geometry;
  bool bounding_box;
  bool silhouette_edges;
  bool shadow_polygons;
  bool gouraud_normals;
  bool glsl_enabled;
  bool glsl_initialized;
  bool two_lights;
  bool using_ai;
  float timer;
  
  AI* theAI;
};

#endif
