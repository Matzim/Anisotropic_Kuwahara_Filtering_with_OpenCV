/*-----------------------------------------------------------------*/
/*!
  \file screen.cpp
  \brief implements screen class
*/
/*-----------------------------------------------------------------*/

#include "screen.hh"

bool Screen::init_screen(GLuint program_filter_id, GLuint program_normal_display_id) {
    this->program_filter_id = program_filter_id;
    this->program_normal_display_id = program_normal_display_id;

    glUseProgram(program_filter_id);
    GLint vertices_location = glGetAttribLocation(program_filter_id, "position");TEST_OPENGL_ERROR();
    GLint uvs_location = glGetAttribLocation(program_filter_id, "uv");TEST_OPENGL_ERROR();

    glGenVertexArrays(1, this->vertex_array_object_id);TEST_OPENGL_ERROR();
    glBindVertexArray(this->vertex_array_object_id[0]);TEST_OPENGL_ERROR();
    glGenBuffers(2, this->vbo_ids);TEST_OPENGL_ERROR();
    
    if (vertices_location != -1) {
        glBindBuffer(GL_ARRAY_BUFFER, this->vbo_ids[0]);TEST_OPENGL_ERROR();
        glBufferData(GL_ARRAY_BUFFER, this->vertex_buffer_data.size() * sizeof(float), vertex_buffer_data.data(), GL_STATIC_DRAW);TEST_OPENGL_ERROR();
        glVertexAttribPointer(vertices_location, 3, GL_FLOAT, GL_FALSE, 0, 0);TEST_OPENGL_ERROR();
        glEnableVertexAttribArray(vertices_location);TEST_OPENGL_ERROR();
    } else {
        std::cerr << "Screen: init_screen: 'position' not found !" << std::endl;
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
    GLint tex_location = glGetUniformLocation(this->program_filter_id, "texture_sampler");TEST_OPENGL_ERROR();
    glUniform1i(tex_location, 0);TEST_OPENGL_ERROR();

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);TEST_OPENGL_ERROR();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);TEST_OPENGL_ERROR();
}

void Screen::init_weights_map(cv::Mat* frame, GLuint weights) {
    glBindTexture(GL_TEXTURE_2D, weights);TEST_OPENGL_ERROR();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, 33, 33, 0, GL_RED, GL_FLOAT, frame->ptr());
    TEST_OPENGL_ERROR();
    GLint tex_location = glGetUniformLocation(this->program_filter_id, "weights");TEST_OPENGL_ERROR();
    glUniform1i(tex_location, 0);TEST_OPENGL_ERROR();

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);TEST_OPENGL_ERROR();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);TEST_OPENGL_ERROR();
}

void Screen::display(bool active_filter) {
    glActiveTexture(GL_TEXTURE0);TEST_OPENGL_ERROR();
    glBindTexture(GL_TEXTURE_2D, this->texture_id);
    // Activate Anisotropic Kuwahara filter or not
    if (active_filter) {
        glUseProgram(this->program_filter_id);TEST_OPENGL_ERROR();
    } else {
        glUseProgram(this->program_normal_display_id);TEST_OPENGL_ERROR();
    }
    glBindVertexArray(this->vertex_array_object_id[0]);TEST_OPENGL_ERROR();
    glDrawArrays(GL_TRIANGLES, 0, this->vertex_buffer_data.size() / 3.0);TEST_OPENGL_ERROR();
    glBindVertexArray(0);TEST_OPENGL_ERROR();
};