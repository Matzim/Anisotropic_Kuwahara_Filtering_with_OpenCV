/*-----------------------------------------------------------------*/
/*!
  \file screen.cpp
  \brief implements screen class
*/
/*-----------------------------------------------------------------*/

#include "screen.hh"

bool Screen::init_screen(GLuint program_id) {
    glUseProgram(program_id);
    this->program_id = program_id;
    GLint vertices_location = glGetAttribLocation(program_id, "position");TEST_OPENGL_ERROR();
    GLint uvs_location = glGetAttribLocation(program_id, "uv");TEST_OPENGL_ERROR();

    glGenVertexArrays(1, &(this->vertex_array_object_id));TEST_OPENGL_ERROR();
    glBindVertexArray(this->vertex_array_object_id);TEST_OPENGL_ERROR();
    glGenBuffers(2, this->vbo_ids);TEST_OPENGL_ERROR();
    
    if (vertices_location != -1) {
        glBindBuffer(GL_ARRAY_BUFFER, this->vbo_ids[0]);TEST_OPENGL_ERROR();
        glBufferData(GL_ARRAY_BUFFER, this->vertex_buffer_data.size() * sizeof(float), vertex_buffer_data.data(), GL_STATIC_DRAW);TEST_OPENGL_ERROR();
        glVertexAttribPointer(vertices_location, 3, GL_FLOAT, GL_FALSE, 0, 0);TEST_OPENGL_ERROR();
        glEnableVertexAttribArray(vertices_location);TEST_OPENGL_ERROR();
    } else {
        std::cerr << "Screen: init_screen: 'position_location' not found !" << std::endl;
        return false;
    }
    if (uvs_location != -1) {
        glBindBuffer(GL_ARRAY_BUFFER, this->vbo_ids[1]);TEST_OPENGL_ERROR();
        glBufferData(GL_ARRAY_BUFFER, this->uv_buffer_data.size() * sizeof(float), uv_buffer_data.data(), GL_STATIC_DRAW);TEST_OPENGL_ERROR();
        glVertexAttribPointer(uvs_location, 2, GL_FLOAT, GL_FALSE, 0, 0);TEST_OPENGL_ERROR();
        glEnableVertexAttribArray(uvs_location);TEST_OPENGL_ERROR();
    }

    glBindVertexArray(0);TEST_OPENGL_ERROR();
    return true;
}

void Screen::init_texture(cv::Mat* frame, GLuint texture) {
    this->texture_id = texture;

    glActiveTexture(GL_TEXTURE0);TEST_OPENGL_ERROR();
    glBindTexture(GL_TEXTURE_2D, this->texture_id);TEST_OPENGL_ERROR();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, frame->cols, frame->rows, 0, GL_BGR, GL_UNSIGNED_BYTE, frame->ptr());
    TEST_OPENGL_ERROR();
    GLint tex_location = glGetUniformLocation(this->program_id, "texture_sampler");TEST_OPENGL_ERROR();
    glUniform1i(tex_location, 0);TEST_OPENGL_ERROR();

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);TEST_OPENGL_ERROR();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);TEST_OPENGL_ERROR();
}

void Screen::display() {
    glUseProgram(this->program_id);TEST_OPENGL_ERROR();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, this->texture_id);

    glBindVertexArray(this->vertex_array_object_id);TEST_OPENGL_ERROR();
    glDrawArrays(GL_TRIANGLES, 0, this->vertex_buffer_data.size() / 3.0);TEST_OPENGL_ERROR();
    glBindVertexArray(0);TEST_OPENGL_ERROR();
};