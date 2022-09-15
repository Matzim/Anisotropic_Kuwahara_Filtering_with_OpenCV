/*! \file main.cpp
    \brief Main fonction of the program
*/

#include <ctime>
#include <iostream>
#include <fstream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>
#include <GL/glew.h>
#include <GL/glut.h>

#include "MultithreadedVideoCapture.hh"
#include "anisotropic_kuwahara/ImagePyramid.hh"
#include "anisotropic_kuwahara/ellipses.hh"
#include "anisotropic_kuwahara_with_opengl/program.hh"
#include "anisotropic_kuwahara_with_opengl/screen.hh"

#define DEFAULT_WIDTH  640
#define DEFAULT_HEIGHT 480

cv::Mat *frame;
unsigned int EBO;
ImagePyramid pyramid = ImagePyramid(3, cv::INTER_LANCZOS4);
unsigned int texture;
program* prog;
Screen *screen;

void GLAPIENTRY MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity,
                                GLsizei length, const GLchar* message, const void* userParam)
{
  (void)source;
  (void)id;
  (void)length;
  (void)userParam;
  fprintf( stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
           (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
            type, severity, message);
}

/**
  Load file content in a string
*/
std::string load(const std::string &filename) {
  std::ifstream input_src_file(filename, std::ios::in);
  std::string ligne;
  std::string file_content = "";
  if (input_src_file.fail()) {
    std::cerr << "Fail in shader loading !" << std::endl;
    return "";
  }
  while (getline(input_src_file, ligne)) {
    file_content = file_content + ligne + "\n";
  }
  file_content += '\0';
  input_src_file.close();
  return file_content;
}

void display() {
  // During init, enable debug ou
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Copy frame into new image
  cv::Mat image = *frame;

  // Pyramid construction is performed using Lanczos3 filter
  pyramid.build_pyramid(image);
  std::vector<cv::Mat> levels = pyramid.get_levels();

  // Apply Anisotropic Kuwahara filter
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, levels[0].cols, levels[0].rows,GL_BGR,GL_UNSIGNED_BYTE, levels[0].ptr());
  glBindTexture(GL_TEXTURE_2D, 0);

  screen->display();

  pyramid.clear_pyramid();

  glutSwapBuffers();
  glutPostRedisplay();
}

int main(int argc, char **argv) {
  glutInit(&argc, argv);
  glutInitContextVersion(4, 5);
  glutInitContextProfile(GLUT_CORE_PROFILE);
  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
  glutInitWindowSize(DEFAULT_WIDTH, DEFAULT_HEIGHT);
  glutInitWindowPosition(0, 0);
  glutCreateWindow("Main");
  glutDisplayFunc(display);

  GLenum err = glewInit();
  if (err != GLEW_OK) {
    std::cout << "Error during GLEW initialization: " << glewGetErrorString(err) << std::endl;
    exit(1);
  }

  // Uncomment this part to get all OpenGL Callbacks
  // glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback(MessageCallback, 0);

  // Construct masks to represent ellipse's subregions
  cv::Mat circle = cv::Mat(23, 23, CV_16S, 0.0);
  create_circle(&circle);
  std::vector<cv::Mat *> masks = get_subregions(circle);
  cv::Mat kernel = cv::Mat(13, 13, CV_64FC1);
  cv::Mat hgrid = cv::Mat(13, 13, CV_64FC1);

  std::string fragment_shader_src = load("src/fragment_shader.shd");
  std::string vertex_shader_src = load("src/vertex_shader.shd");

  prog = program::make_program(vertex_shader_src, fragment_shader_src);

  Screen init = Screen();
  screen = &init;
  screen->init_screen(prog->getProgID());

  for (int i = 0; i < 13; i++) {
    for (int j = 0; j < 13; j++) {
      kernel.at<double>(i, j) = static_cast<double>(i - 6.0);
      hgrid.at<double>(i, j) = static_cast<double>(j - 6.0);
    }
  }

  cv::pow(kernel, 2, kernel);
  cv::pow(hgrid, 2, hgrid);
  cv::add(kernel, hgrid, kernel);
  kernel /= -8.0;
  cv::exp(kernel, kernel);
  kernel *= (1.0 / (8.0 * CV_PI));

  if (argc > 1) {
    // Process images
    for (size_t i = 1; argv[i] != nullptr; i++) {
      cv::Mat input = cv::imread(argv[i], cv::IMREAD_COLOR);

      // Apply Anisotropic Kuwahara filter

      // Display resultat
      cv::waitKey(0);
    }
    for (int i = 0; i < NB_SUBREGIONS; i++) {
      delete masks.at(i);
    }
    return 0;
  }

  // Use camera when no arguments are provided to the program
  cv::VideoCapture camera;  // Initialize VideoCapture

  // Open default camera and autodetect default API
  camera.open(0, cv::CAP_ANY);

  // Check if we succeeded
  if (!camera.isOpened()) {
    std::cerr << "ERROR ! Unable to open camera !" << std::endl;
    return -1;
  }
  camera.set(cv::CAP_PROP_FRAME_WIDTH, DEFAULT_WIDTH);
  camera.set(cv::CAP_PROP_FRAME_HEIGHT, DEFAULT_HEIGHT);

  std::cout << "You are using " << camera.getBackendName() << std::endl;

  MultithreadedVideoCapture cam(&camera);
  cam.start();

  // Get a pointer that store next frame
  frame = cam.read();

  glGenTextures(1, &texture);

  screen->init_texture(frame, texture);

  glutMainLoop();

  cam.stop();
  for (int i = 0; i < NB_SUBREGIONS; i++) {
    delete masks.at(i);
  }
  return 0;
}