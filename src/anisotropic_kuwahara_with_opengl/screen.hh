/*-----------------------------------------------------------------*/
/*!
  \file screen.hh
  \brief Image is mapped to the 'screen'
*/
/*-----------------------------------------------------------------*/
#ifndef __SCREEN__
#define __SCREEN__

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <opencv2/core.hpp>
#include <vector>
#include <string>
#include <iostream>

#define TEST_OPENGL_ERROR()                                                    \
  do {                                                                         \
    GLenum err = glGetError();                                                 \
    if (err != GL_NO_ERROR)                                                    \
      std::cerr << "OpenGL ERROR!" << __LINE__ << std::endl;                   \
  } while (0)

class Screen {
    public:
    Screen() {
        this->vertex_buffer_data = std::vector<GLfloat>({
            -1.0f, -1.0f, 0.0f,
            -1.0f, 1.0f, 0.0f,
            1.0f, -1.0f, 0.0f,
            -1.0f, 1.0f, 0.0f,
            1.0f, -1.0f, 0.0f,
            1.0f, 1.0f, 0.0f
        });
        this->uv_buffer_data = std::vector<GLfloat>({
            1.0f, 1.0f,
            1.0f, 0.0f,
            0.0f, 1.0f,
            1.0f, 0.0f,
            0.0f, 1.0f,
            0.0f, 0.0f
        });
    };

    ~Screen() {
        glDeleteTextures(1, &this->texture_id);
        glDeleteVertexArrays(1, this->vertex_array_object_id);
        glDeleteBuffers(2, this->vbo_ids);
    };
    void display(bool active_filter);

    bool init_screen(GLuint program_filter_id, GLuint program_normal_display_id);
    void init_texture(cv::Mat* frame, GLuint texture);
    void init_weights_map(cv::Mat* frame, GLuint weights);

    protected:
    std::vector<GLfloat> vertex_buffer_data;
    std::vector<GLfloat> uv_buffer_data;

    GLuint vertex_array_object_id[1];
    GLuint texture_id = 0;

    GLuint vbo_ids[2];
    GLuint program_filter_id;
    GLuint program_normal_display_id;
};

#endif // !__SCREEN__