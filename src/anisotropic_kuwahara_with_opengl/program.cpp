/*-----------------------------------------------------------------*/
/*!
  \file program.cpp
  \brief implements program class
*/
/*-----------------------------------------------------------------*/

#include "program.hh"

#include <iostream>

program::program() {
  shaders_id[0] = glCreateShader(GL_VERTEX_SHADER);
  TEST_OPENGL_ERROR();
  shaders_id[1] = glCreateShader(GL_FRAGMENT_SHADER);
  TEST_OPENGL_ERROR();
  prog_id = glCreateProgram();
  TEST_OPENGL_ERROR();
}

program::~program() {
  glDeleteProgram(prog_id);
  TEST_OPENGL_ERROR();
  glDeleteShader(shaders_id[0]);
  TEST_OPENGL_ERROR();
  glDeleteShader(shaders_id[1]);
  TEST_OPENGL_ERROR();
}

program *program::make_program(std::string &vertex_shader_src,
                               std::string &fragment_shader_src) {
  program *res = new program();

  glShaderSource(res->shaders_id[0], 1, (const GLchar **)&vertex_shader_src,
                 nullptr);
  TEST_OPENGL_ERROR();

  glShaderSource(res->shaders_id[1], 1, (const GLchar **)&fragment_shader_src,
                 nullptr);
  TEST_OPENGL_ERROR();

  glCompileShader(res->shaders_id[0]);
  TEST_OPENGL_ERROR();

  GLint vertex = 0;
  glGetShaderiv(res->shaders_id[0], GL_COMPILE_STATUS, &vertex);
  if (vertex == GL_FALSE) {
    std::cerr << "There was an error when compiling vertex shaders\n";
    return res;
  }

  glCompileShader(res->shaders_id[1]);
  TEST_OPENGL_ERROR();

  GLint fragment = 0;
  glGetShaderiv(res->shaders_id[1], GL_COMPILE_STATUS, &fragment);
  if (fragment == GL_FALSE) {
    std::cerr << "There was an error when compiling fragment shaders !" << std::endl;
    return res;
  }

  glAttachShader(res->prog_id, res->shaders_id[0]);
  TEST_OPENGL_ERROR();

  glAttachShader(res->prog_id, res->shaders_id[1]);
  TEST_OPENGL_ERROR();

  glLinkProgram(res->prog_id);
  TEST_OPENGL_ERROR();

  GLint isLinked = 0;
  glGetProgramiv(res->prog_id, GL_LINK_STATUS, (int *)&isLinked);
  if (isLinked == GL_FALSE) {
    std::cerr << "There was an error when linking program\n";
    return res;
  }

  glDetachShader(res->prog_id, res->shaders_id[0]);
  TEST_OPENGL_ERROR();

  glDetachShader(res->prog_id, res->shaders_id[1]);
  TEST_OPENGL_ERROR();

  return res;
}

char *program::get_log() {
  GLint vertex = 0;
  glGetShaderiv(shaders_id[0], GL_COMPILE_STATUS, &vertex);

  GLint fragment = 0;
  glGetShaderiv(shaders_id[1], GL_COMPILE_STATUS, &fragment);

  GLint isLinked = 0;
  glGetProgramiv(prog_id, GL_LINK_STATUS, (int *)&isLinked);

  if (vertex == GL_FALSE) {
    GLint length = 0;
    glGetShaderiv(shaders_id[0], GL_INFO_LOG_LENGTH, &length);
    char *res = (char *)malloc(sizeof(char) * (length + 1));
    glGetShaderInfoLog(shaders_id[0], length, &length, &res[0]);
    return res;
  }

  if (fragment == GL_FALSE) {
    GLint length = 0;
    glGetShaderiv(shaders_id[1], GL_INFO_LOG_LENGTH, &length);
    char *res = (char *)malloc(sizeof(char) * (length + 1));
    glGetShaderInfoLog(shaders_id[1], length, &length, &res[0]);
    return res;
  }

  if (isLinked == GL_FALSE) {
    GLint length = 0;
    glGetProgramiv(prog_id, GL_INFO_LOG_LENGTH, &length);
    char *res = (char *)malloc(sizeof(char) * (length + 1));
    glGetProgramInfoLog(prog_id, length, &length, &res[0]);
    return res;
  }

  return nullptr;
}

bool program::is_ready() {
  char *log = get_log();
  bool res = true;

  if (log != nullptr) {
    std::cout << log << std::endl;
    res = false;
    free(log);
  }

  return res;
}

void program::use() { glUseProgram(prog_id); }