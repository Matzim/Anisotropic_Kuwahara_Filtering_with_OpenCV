/*-----------------------------------------------------------------*/
/*!
  \file program.hh
  \brief defines matrix class and utlity functions
*/
/*-----------------------------------------------------------------*/

#ifndef __PROGRAM__
#define __PROGRAM__

#include <GL/glew.h>

#include <GL/freeglut.h>
#include <string>

#define TEST_OPENGL_ERROR()                                                    \
  do {                                                                         \
    GLenum err = glGetError();                                                 \
    if (err != GL_NO_ERROR)                                                    \
      std::cerr << "OpenGL ERROR!" << __LINE__ << std::endl;                   \
  } while (0)

class program {
public:
  program();
  ~program();
  static program *make_program(std::string &vertex_shader_src,
                               std::string &fragment_shader_src);
  char *get_log();
  bool is_ready();
  void use();
  GLuint getProgID() { return prog_id; }

private:
  GLint shaders_id[2];
  GLuint prog_id;
};

#endif