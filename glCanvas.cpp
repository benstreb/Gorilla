#include "glCanvas.h"
#include "camera.h"

#if _WIN32
#include <Winsock2.h>
#else
#include <sys/time.h>
#endif

#include "mesh.h"
#include "argparser.h"
#include "matrix.h"

#ifndef M_PI
#define M_PI 3.14159
#endif

// ========================================================
// static variables of GLCanvas class

int GLCanvas::mouseButton;
int GLCanvas::mouseX;
int GLCanvas::mouseY;
//int GLCanvas::display_list_index;
ArgParser* GLCanvas::args;
Mesh* GLCanvas::mesh;
PerspectiveCamera* GLCanvas::camera;
bool GLCanvas::controlPressed;
bool GLCanvas::animate;
GLhandleARB GLCanvas::program;
GLhandleARB GLCanvas::vertex_shader;
GLhandleARB GLCanvas::fragment_shader;

// ========================================================

void GLCanvas::InitLight() {
  // Set the last component of the position to 0 to indicate
  // a directional light source

  GLfloat position[4] = { 60.0f,60.0f,200.0f,1.0f};
  GLfloat diffuse[4] = { 0.7f,0.7f,0.7f,1.0f};
  GLfloat specular[4] = { 0.0f,0.0f,0.0f,1.0f};
  GLfloat ambient[4] = { 0.3f, 0.3f, 0.3f, 1.0f };
  /*GLfloat position[4] = { 15.0f,15.0f,50.0f,1.0f};
  GLfloat diffuse[4] = { 0.35f,0.35f,0.35f,1.0f};
  GLfloat specular[4] = { 0.0f,0.0f,0.0f,1.0f};
  GLfloat ambient[4] = { 0.15f, 0.15f, 0.15f, 1.0f };*/

  GLfloat zero[4] = {0,0,0,0};
  glLightfv(GL_LIGHT1, GL_POSITION, position);
  glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse);
  glLightfv(GL_LIGHT1, GL_SPECULAR, specular);
  glLightfv(GL_LIGHT1, GL_AMBIENT, zero);
  glEnable(GL_LIGHT1);

  glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
  glEnable(GL_COLOR_MATERIAL);
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);

  GLfloat spec_mat[4] = {1.0f,1.0f,1.0f,1.0f};
  float glexponent = 30;
  glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, &glexponent);
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spec_mat);

  glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
  float back_color[] = { 0.1f,0.1f,0.1f,1.0f};
  glMaterialfv(GL_BACK, GL_AMBIENT_AND_DIFFUSE, back_color);
  glEnable(GL_LIGHT1);
}


void GLCanvas::display(void)
{
  // Clear the display buffer, set it to the background color
  glClearColor(1,1,1,0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Set the camera parameters
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  InitLight(); // light will be a headlamp!
  camera->glPlaceCamera();

  glEnable(GL_LIGHTING);
  glEnable(GL_DEPTH_TEST);
  mesh->drawVBOs();
     
  // Swap the back buffer with the front buffer to display
  // the scene
  glutSwapBuffers();
}

// ========================================================
// Callback function for window resize
// ========================================================

void GLCanvas::reshape(int w, int h) {
  // Set the OpenGL viewport to fill the entire window
  glViewport(0, 0, (GLsizei)w, (GLsizei)h);

  // Set the camera parameters to reflect the changes
  camera->glInit(w, h);

  args->width = w;
  args->height = h;
}

// ========================================================
// Callback function for mouse click or release
// ========================================================

void GLCanvas::mouse(int button, int state, int x, int y) {
  // Save the current state of the mouse.  This will be
  // used by the 'motion' function
  mouseButton = button;
  mouseX = x;
  mouseY = y;
  controlPressed = (glutGetModifiers() & GLUT_ACTIVE_CTRL) != 0;
  
  //std::cout << "mouse: " << state << std::endl;
  
  //mouseButton == GLUT_LEFT_BUTTON && !mesh->editBoard().applySpeculativePiece();
  
  GoBoard* theBoard = &(mesh->editBoard());
  
  if(mouseButton == GLUT_LEFT_BUTTON && state == 0) {
    bool success = true;
    if(!args->ai_vs_ai) {
      success = mesh->editBoard().applySpeculativePiece();
    } else {
      coord ai1_move = args->theAI->getMove(theBoard);
      if(ai1_move == coord(-1,-1)) {
        if(theBoard->getJustPassed()) {
          theBoard->endOfGame();
          success = false;
        } else {
          theBoard->passTurn();
        }
      } else {
        theBoard->makeMove(theBoard->getTurn(), ai1_move.x, ai1_move.y);
        theBoard->nextTurn();
      }
    }
    mouseButton = GLUT_LEFT_BUTTON && !success;
    
    if(success) {
      if(args->using_ai) {      
        coord ai_move = args->theAI->getMove(theBoard);
        if(ai_move == coord(-1,-1)) {
          if(theBoard->getJustPassed()) {
            theBoard->endOfGame();
          } else {
            theBoard->passTurn();
          }
        } else {
          theBoard->makeMove(theBoard->getTurn(), ai_move.x, ai_move.y);
          theBoard->nextTurn();
        }
      } else if(args->ai_vs_ai) {
        coord ai2_move = args->theAI_2->getMove(theBoard);
        if(ai2_move == coord(-1,-1)) {
          theBoard->passTurn();
        } else {
          theBoard->makeMove(theBoard->getTurn(), ai2_move.x, ai2_move.y);
          theBoard->nextTurn();
        }        
      }
    }
  }
  else if(mouseButton == GLUT_RIGHT_BUTTON && state == 0) {
    std::cout << "SKIP MY TURN" << std::endl;
    if(theBoard->getJustPassed()) {
      theBoard->endOfGame();
    } else {
      theBoard->passTurn();

      if(args->using_ai) {
        coord ai_move = args->theAI->getMove(theBoard);
        if(ai_move == coord(-1,-1)) {
          theBoard->endOfGame();
        } else {
          theBoard->makeMove(theBoard->getTurn(), ai_move.x, ai_move.y);
          theBoard->nextTurn();
        }
      }
    }
  }
}

// ========================================================
// Callback function for mouse drag
// ========================================================

void GLCanvas::motion(int x, int y) {
  // Left button = rotation
  // (rotate camera around the up and horizontal vectors)
  if (mouseButton == GLUT_LEFT_BUTTON) {
  /*
    camera->rotateCamera(0.005*(mouseX-x), 0.005*(mouseY-y));
    mouseX = x;
    mouseY = y;
    */
  }
  // Middle button = translation
  // (move camera perpendicular to the direction vector)
  else if (mouseButton == GLUT_MIDDLE_BUTTON) {
    
    camera->truckCamera(mouseX-x, y-mouseY);
    mouseX = x;
    mouseY = y;
    
  }
  // Right button = dolly or zoom
  // (move camera along the direction vector)
  else if (mouseButton == GLUT_RIGHT_BUTTON) {
    /*
    if (controlPressed) {
      camera->zoomCamera(mouseY-y);
    } else {
      camera->dollyCamera(mouseY-y);
    }
    mouseX = x;
    mouseY = y;
    */
  }
  

  // Redraw the scene with the new camera parameters
  glutPostRedisplay();
}

Matrix fromVec3fs(const Vec3f &a, const Vec3f &b, const Vec3f &c) {
  Matrix m;
  for (int i = 0; i < 3; i++) {
    m.set(0, i, a[i]);
    m.set(1, i, b[i]);
    m.set(2, i, c[i]);
  }
    m.set(3, 3, 1);
  return m;
}

void GLCanvas::mouseRay(int mouseX, int mouseY) {
  auto square_hit = std::make_pair(-1, -1);
  Vec3f camPos = camera->camera_position;


  Vec3f screenCenter = camPos - camera->getDirection();
  double screenHeight = 2 * tan(camera->getAngle()/2.0);
  Vec3f xAxis = camera->getHorizontal() * screenHeight;
  Vec3f yAxis = camera->getScreenUp() * screenHeight;
  Vec3f lowerLeft = screenCenter - 0.5*xAxis - 0.5*yAxis;
  Vec3f screenPoint = lowerLeft + float(mouseX)/args->width * xAxis + float(mouseY)/args->height * yAxis;
  Vec3f dir = camPos - screenPoint;
  Vec3f modDir = dir;
  modDir /= dir.y();
  modDir *= camPos.y() - 1;

  //Vec3f t = camPos - modDir;
  /*glTranslatef(t.x() - .4*dir.x(), t.y() - .4*dir.y(), t.z() - .4*dir.z());
  float color = 0.0;
  for (int i = 0; i < 10; i++) {
    glColor3f(color, color, color);
    glTranslatef(-.1*dir.x(), -.1*dir.y(), -.1*dir.z());
    glutSolidSphere(1, 5, 5);
    color += 0.1;
  }
  glTranslatef(-t.x() - .6*dir.x(), -t.y() - .6*dir.y(), -t.z() - .6*dir.z());*/

  Vec3f intersection = (camPos - modDir)*10;

  square_hit = std::make_pair(-int(floor(intersection[0])), int(floor(intersection[2])));
  //printf("%d, %d\n", square_hit.first, square_hit.second);

  mesh->editBoard().placeSpeculativePiece(1, square_hit.first, square_hit.second);
  //printf("%f, %f, %f\n", a[0], a[1], a[2]);
}

// =============================================================
// =============================================================

// this function gets executed repeatedly and allows us to animate the
// light source.  stats on the frames per sec are also printed.
void GLCanvas::idle() {
#if _WIN32    
  static int last_tick_count;
  static int last_fps_count;
  static int frames = -1;
  if (frames == -1) {    
    last_fps_count = last_tick_count = GetTickCount();
    frames = 0;
  }
  if (animate) {
    frames++;
    int this_tick_count = GetTickCount();
    // difference in milliseconds
    double diff = 0.001*(this_tick_count-last_tick_count);
    last_tick_count = this_tick_count;
    args->timer += diff;
    double diff_fps_time = 0.001*(this_tick_count-last_fps_count);
    if (diff_fps_time > 1.00) {      
      float fps = frames / float(diff_fps_time);
      std::cout << "fps: " << fps << std::endl;
      frames = 0;
      last_fps_count = this_tick_count;
    }
    mesh->setupVBOs();
    glutPostRedisplay();
  } else {
    last_tick_count = last_fps_count = GetTickCount();
  }
#else
  static timeval last_time;
  static timeval last_fps_time;
  static int frames = -1;
  if (frames == -1) {
    gettimeofday(&last_time,NULL);
    last_fps_time = last_time;
    frames = 0;
  }
  if (animate) {
    frames++;
    timeval this_time;
    gettimeofday(&this_time,NULL);
    // compute the difference from last time
    double diff = this_time.tv_sec - last_time.tv_sec + 
      0.000001 * (this_time.tv_usec - last_time.tv_usec);
    double diff_fps_time = this_time.tv_sec - last_fps_time.tv_sec + 
      0.000001 * (this_time.tv_usec - last_fps_time.tv_usec);
    last_time = this_time;
    // print out stats on the FPS occasionally
    if (diff_fps_time > 1.00) {      
      float fps = frames / float(diff_fps_time);
      std::cout << "fps: " << fps << std::endl;
      frames = 0;
      last_fps_time = this_time;
    }
    args->timer += diff;
    mesh->setupVBOs();
    glutPostRedisplay();

  } else {
    // if we aren't animating the light source, avoid busy-waiting!
    gettimeofday(&last_time, NULL);
    last_fps_time = last_time;
    frames = 0;
    usleep (100);
  }
#endif
}

// ========================================================
// Callback function for keyboard events
// ========================================================

void GLCanvas::keyboard(unsigned char key, int /*x*/, int /*y*/) {
  switch (key) {
  case 'c': case 'C':
    args->board_control = !args->board_control;
    mesh->setupVBOs();
    break;
  case 'b': case 'B':
    args->bounding_box = !args->bounding_box;
    mesh->setupVBOs();
    break;
  case 'g': case 'G':
    args->geometry = !args->geometry;
    mesh->setupVBOs();
    break;
  case 'm': case 'M':
    args->mirror = !args->mirror;
    mesh->setupVBOs();
    break;
  case 'r': case 'R':
    mesh->resetBoard();
    mesh->setupVBOs();
    break;
  case 's': case 'S':
    args->shadow = !args->shadow;
    mesh->setupVBOs();
    break;
  case 'e': case 'E':
  {
    if(args->using_evaluator)
    {
      GoBoard* theBoard = &(mesh->editBoard());
      std::pair<coord, int> speculation;
      speculation = theBoard->getSpeculativePiece();
      coord spec_loc;
      spec_loc = speculation.first;
      int playa;
      playa = theBoard->getTurn();
      std::cout << "Evaluate position: " << spec_loc.x << ", " << spec_loc.y << " for player: " << playa << std::endl;
      double res = 0;
      res = ((AI_Monte_Carlo*)args->evaluator)->evaluateMove(theBoard, playa, spec_loc);
      std::cout << "\t" << res << std::endl;
    }
  }
    break;
  case 'p': case 'P':
    args->shadow_polygons = !args->shadow_polygons;
    mesh->setupVBOs();
    break;
  case 'a': case 'A':
    animate = !animate;
    break;
  case ' ':
    args->timer += 0.1f;
    mesh->setupVBOs();
    break;
  case 'n': case 'N':
    args->gouraud_normals = !args->gouraud_normals;
    mesh->setupVBOs();
    break;
  case 'x': case 'X':
    args->glsl_enabled = !args->glsl_enabled;
    if (args->glsl_enabled) {
      if (!args->glsl_initialized) {
  // initialize the shaders
  InitShaders();
  args->glsl_initialized = true;
      }
      mesh->generateShadowFBO();
      mesh->shadowMapUniform = LoadCompileLinkShaders();
    }
    else
    {
      glDisable(GL_CULL_FACE);
      glDisable(GL_STENCIL_TEST);
      glDisable(GL_DEPTH_TEST);
      glClearColor(0,0,0,1.0f);
    }
    mesh->setupVBOs();
    break;
  case 'l' : case 'L':
    if (args->glsl_enabled) {
      assert (args->glsl_initialized);
      LoadCompileLinkShaders();
      mesh->setupVBOs();
    }
    break;
  case 'q':  case 'Q':
    exit(0);
    break;
  default:
    printf("UNKNOWN KEYBOARD INPUT  '%c'\n", key);
  }
  glutPostRedisplay();
}

// ========================================================
// Initialize all appropriate OpenGL variables, set
// callback functions, and start the main event loop.
// This function will not return but can be terminated
// by calling 'exit(0)'
// ========================================================

void GLCanvas::initialize(ArgParser *_args, Mesh *_mesh) {
  args = _args;
  mesh = _mesh;
  Vec3f camera_position = Vec3f(0,10,10);
  Vec3f point_of_interest = Vec3f(0,0,0);
  Vec3f up = Vec3f(0,1,0);
  camera = new PerspectiveCamera(camera_position, point_of_interest, up, 20 * M_PI/180.0);
  mesh->camera = camera;
  // Set window parameters
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB | GLUT_STENCIL );
  glutInitWindowSize(args->width, args->height);
  glutInitWindowPosition(100,100);
  glutCreateWindow("OpenGL Viewer");
  int num_bits;
  glGetIntegerv(GL_STENCIL_BITS,&num_bits);
  if (num_bits < 8) {
    std::cout << "WARNING: stencil buffer is only " << num_bits << " bits." << std::endl;
  }
#ifdef _WIN32
  GLenum err = glewInit();
  if (err != GLEW_OK) {
      fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
      exit(1);
  }
#endif

  glEnable(GL_DEPTH_TEST);

  // Set global lighting parameters
  glEnable(GL_LIGHTING);
  glShadeModel(GL_SMOOTH);

  glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
  glEnable(GL_NORMALIZE);
  // Ambient light
  Vec3f ambColor = Vec3f(0.2,0.2,0.2); 
  GLfloat ambArr[] = { float(ambColor.x()), float(ambColor.y()), float(ambColor.z()), 1.0 };
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambArr);

  glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
  glCullFace(GL_BACK);
  glDisable(GL_CULL_FACE);
  //display_list_index = glGenLists(1);

  // Initialize callback functions
  glutMouseFunc(mouse);
  glutMotionFunc(motion);
  glutPassiveMotionFunc(mouseRay);
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutKeyboardFunc(keyboard);
  glutIdleFunc(display);

  mesh->initializeVBOs();
  mesh->setupVBOs();
  // Enter the main rendering loop
  glutMainLoop();
}

// ========================================================
// ========================================================


void HandleGLError(std::string msg) {
  GLenum error;
  int i = 0;
  while ((error = glGetError()) != GL_NO_ERROR) {
    printf ("GL ERROR(%d):  %s\n", i, gluErrorString(error));
    i++;
  }
  if (i != 0) {
    std::cout << msg << std::endl;
    exit(0);
  }
}

