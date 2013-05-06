#ifndef __ARG_PARSER_H__
#define __ARG_PARSER_H__

#include <cstdio>
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <string>

#include "ai.h"
#include "ai_random.h"
#include "ai_monte_carlo.h"

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
      } else if (!strcmp(argv[i],"-eval")) {
		using_evaluator = true;
		i++; assert (i < argc);
		int games = atoi(argv[i]);
		i++; assert (i < argc);
		int turns = atoi(argv[i]);
		
		evaluator = new AI_Monte_Carlo(0, games, turns);
      } else if (!strcmp(argv[i],"-no_gui")) {
		gui = false;
      }
      else if (!strcmp(argv[i],"-ai_vs_ai")) {
		assert(!using_ai);
		ai_vs_ai = true;
		
      } else if (!strcmp(argv[i],"-ai_random")) {
		if(!ai_vs_ai)
		{
			theAI = new AI_Random(1);
			using_ai = true;
		}
		else
		{
			assert(ai_id < 2);
			if(ai_id == 0)
			{
				theAI = new AI_Random(-1);
				ai_id++;
			}
			else
			{
				theAI_2 = new AI_Random(1);
				ai_id++;
			}
		}
      } else if (!strcmp(argv[i],"-ai_monte_carlo")) {
		i++; assert (i < argc);
		int games = atoi(argv[i]);
		i++; assert (i < argc);
		int turns = atoi(argv[i]);
		if(!ai_vs_ai)
		{
			theAI = new AI_Monte_Carlo(1, games, turns);
			using_ai = true;			
		}
		else
		{
			assert(ai_id < 2);
			if(ai_id == 0)
			{
				theAI = new AI_Monte_Carlo(-1, games, turns);
				ai_id++;
			}
			else
			{
				theAI_2 = new AI_Monte_Carlo(1, games, turns);
				ai_id++;
			}
		}
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
    ai_id = 0;
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
    gui = true;
    timer = 0.0;
    using_ai = false;
    ai_vs_ai = false;
    using_evaluator = false;
  }

  // ==============
  // REPRESENTATION
  // all public! (no accessors)

  std::string input_file;
  std::string fragment_shader_filename;
  std::string vertex_shader_filename;
  int width;
  int height;
  int ai_id;
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
  bool gui;
  bool using_ai;
  bool ai_vs_ai;
  bool using_evaluator;
  float timer;
  
  AI* theAI;
  AI* theAI_2;
 
  AI* evaluator;
};

#endif
