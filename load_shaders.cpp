#include "glCanvas.h"
#include <iostream>
#include <fstream>
#include "argparser.h"

void CheckForGLSL() {
#if _WIN32
  // Missing code for the Windows equivalent :(
#else
  const GLubyte* extensions = glGetString(GL_EXTENSIONS);
  if (!gluCheckExtension((const GLubyte*)"GL_ARB_shader_objects", extensions) ||
      !gluCheckExtension((const GLubyte*)"GL_ARB_vertex_shader", extensions) ||
      !gluCheckExtension((const GLubyte*)"GL_ARB_fragment_shader", extensions) ||
      !gluCheckExtension((const GLubyte*)"GL_ARB_shading_language_100", extensions)) {
    std::cout << glGetString(GL_RENDERER) << " does not support GLSL" << std::endl;
    exit(1);
  }
#endif
}

void CompileProgram(GLcharARB* sourcecode, GLhandleARB *shader) {
  GLint	logLength;
  GLint	compiled;
  glShaderSourceARB(*shader,1,(const GLcharARB **)&sourcecode,0);
  HandleGLError("Failed glShaderSourceARB");
  glCompileShaderARB(*shader);
  HandleGLError("Failed glCompileShaderARB");
  glGetObjectParameterivARB(*shader,GL_OBJECT_COMPILE_STATUS_ARB,&compiled);
  glGetObjectParameterivARB(*shader,GL_OBJECT_INFO_LOG_LENGTH_ARB,&logLength);
  if (logLength) {
    GLcharARB *log = (GLcharARB *)malloc(logLength+128);
    glGetInfoLogARB(*shader, logLength, &logLength, log);
    printf("Compile log: \n");
    free (log);
  }
  if (!compiled) {
    std::cout << "shader could not compile" << std::endl;
    exit(0);
  }
}

void LinkProgram(GLhandleARB program) {
  GLint	logLength;
  GLint linked;
  glLinkProgramARB(program);
  HandleGLError("Failed glLinkProgramARB");
  glGetObjectParameterivARB(program,GL_OBJECT_LINK_STATUS_ARB,&linked);
  glGetObjectParameterivARB(program,GL_OBJECT_INFO_LOG_LENGTH_ARB,&logLength);
  if (logLength) {
    GLint charsWritten;
    GLcharARB *log = (GLcharARB*)malloc(logLength+128);
    glGetInfoLogARB(program, logLength, &charsWritten, log);
    printf("Link GetInfoLogARB:\n%s\n",log);
    free (log);
  }
  if (!linked)  {
    std::cout << "shader did not link" << std::endl;
    exit(0);
  }
}

std::string ReadFile(const std::string &filename) {
  std::string answer;
  std::ifstream file(filename.c_str());
  assert (file);
  std::string s;
  while (getline(file,s)) {
    answer += s + "\n";
  }
  return answer;
}

void GLCanvas::InitShaders() {
  std::cout << "initialize shaders" << std::endl;
  CheckForGLSL();
  program = glCreateProgramObjectARB();
  vertex_shader = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
  fragment_shader = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);
  HandleGLError("Failed to create program or shader objects");
  glAttachObjectARB(program,vertex_shader);
  glAttachObjectARB(program,fragment_shader);
  HandleGLError("Failed to attach shaders to program");
}

void GLCanvas::LoadCompileLinkShaders() {
  std::cout << "load, compile, & link shaders" << std::endl;
  std::string v_string = ReadFile(args->vertex_shader_filename);
  char *v = (char*) v_string.c_str();
  std::string f_string = ReadFile(args->fragment_shader_filename);
  char *f = (char*) f_string.c_str();
  CompileProgram(v, &vertex_shader);
  CompileProgram(f, &fragment_shader);
  LinkProgram(program);
  HandleGLError("Failed to compile or link shaders");
}

