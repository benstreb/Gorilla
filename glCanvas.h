#ifndef _GL_CANVAS_H_
#define _GL_CANVAS_H_

#include <cstdio>
#include <cstdlib>
#include <string>

// Included files for OpenGL Rendering
#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#include <OpenGL/glext.h>
#else
#define GL_GLEXT_PROTOTYPES
#ifdef _WIN32
#define GLUT_DISABLE_ATEXIT_HACK
#include <GL/glew.h>
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "GL/wglew.h"
#else
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>
#include <GL/glx.h>
#include <GL/glxext.h>
#endif
#endif

#include <cassert>

class ArgParser;
class IFS;
class Mesh;
class Camera;

// ====================================================================
// NOTE:  All the methods and variables of this class are static
// ====================================================================

class GLCanvas {

public:

  // Set up the canvas and enter the rendering loop
  // Note that this function will not return but can be
  // terminated by calling 'exit(0)'
  static void initialize(ArgParser *_args, Mesh *_mesh);

private:

  static void InitLight();
  static void InitShaders();
  static void LoadCompileLinkShaders();

  // various static variables
  static ArgParser *args;
  static Mesh *mesh;
  static Camera* camera;
  static int display_list_index;
  // state of the mouse cursor
  static int mouseButton;
  static int mouseX;
  static int mouseY;
  static bool controlPressed;
  static bool animate;
  // Callback functions for mouse and keyboard events
  static void display(void);
  static void reshape(int w, int h);
  static void mouse(int button, int state, int x, int y);
  static void motion(int x, int y);
  static void keyboard(unsigned char key, int x, int y);
  static void idle();

 public:
  static GLhandleARB program;
 private:
  static GLhandleARB vertex_shader;
  static GLhandleARB fragment_shader;
};

// ====================================================================

void HandleGLError(std::string msg="");

#endif

