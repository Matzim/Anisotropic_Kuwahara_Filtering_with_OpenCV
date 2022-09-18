/*-----------------------------------------------------------------*/
/*! \file main.cpp
    \brief Main fonction of the program
*/
/*-----------------------------------------------------------------*/
#include <iostream>
#include <fstream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/imgproc.hpp>
#include <GL/glew.h>
#include <GL/glut.h>

#include "MultithreadedVideoCapture.hh"
#include "anisotropic_kuwahara/utils.hh"
#include "anisotropic_kuwahara_with_opengl/program.hh"
#include "anisotropic_kuwahara_with_opengl/screen.hh"

#define DEFAULT_WIDTH  640
#define DEFAULT_HEIGHT 480

cv::Mat *frame;
unsigned int texture;
Screen *screen;
std::vector<cv::Mat*> input_images;
unsigned int index_image;
bool active_filter;

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
** Load file content in a string
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

/**
** Display callback for the current window
*/
void display() {
  // During init, enable debug ou
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Copy frame into new OpenCV matrix
  cv::Mat image = *frame;

  // Apply Anisotropic Kuwahara filter
  glBindTexture(GL_TEXTURE_2D, texture);
  screen->init_texture(&image, texture);
  glBindTexture(GL_TEXTURE_2D, 0);

  screen->display(active_filter);

  glutSwapBuffers();
  glutPostRedisplay();
}

/**
** Desactivate or active filter, when user press space bar
*/
void activate_filter(unsigned char key, int x, int y) {
  (void) x;
  (void) y;
  switch(key)
  {
    case ' ':
      active_filter = !active_filter;
      glutPostRedisplay();
    break;
  }
}

/**
** Keyboard callback for the current window to render another image
*/
void change_image(unsigned char key, int x, int y) {
  (void) x;
  (void) y;
  switch(key)
  {
    case GLUT_KEY_UP:
    case GLUT_KEY_RIGHT:
      if (index_image < input_images.size() - 1) {
        index_image++;
      } else {
        index_image = 0;
      }
      glutReshapeWindow(input_images[index_image]->cols, input_images[index_image]->rows);
      glutPostRedisplay();
      break;
    case GLUT_KEY_DOWN:
    case GLUT_KEY_LEFT:
      if (index_image > 0) {
        index_image--;
      } else {
        index_image = input_images.size() - 1;
      }
      glutReshapeWindow(input_images[index_image]->cols, input_images[index_image]->rows);
      glutPostRedisplay();
      break;
    case ' ':
      active_filter = !active_filter;
      glutPostRedisplay();
      break;
  }
}

/**
** Display callback for the current window
*/
void display_images() {
  // Clear OpenGL Window
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Copy image into new OpenCV matrix
  cv::Mat image = *input_images[index_image];

  // Apply Anisotropic Kuwahara filter
  glBindTexture(GL_TEXTURE_2D, texture);
  screen->init_texture(&image, texture);
  glBindTexture(GL_TEXTURE_2D, 0);

  screen->display(active_filter);

  glutSwapBuffers();
}

int main(int argc, char **argv) {
  // Initialize OpenGL / GLUT context
  glutInit(&argc, argv);
  glutInitContextVersion(4, 5);
  glutInitContextProfile(GLUT_CORE_PROFILE);
  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
  glutInitWindowSize(DEFAULT_WIDTH, DEFAULT_HEIGHT);
  glutInitWindowPosition(0, 0);
  glutCreateWindow("Anisotropic Kuwahara");

  GLenum err = glewInit();
  if (err != GLEW_OK) {
    std::cout << "Error: GLEW initialization: " << glewGetErrorString(err) << std::endl;
    exit(1);
  }

  // Uncomment this part to get all OpenGL Callbacks
  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback(MessageCallback, 0);

  double data[1089] = {
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
  };
  cv::Mat circle = cv::Mat(33, 33, CV_64FC1, data);

  cv::Mat kernel = cv::Mat(7, 7, CV_64FC1, 0.0);
  double coeff = 1.0 / (2.0 * CV_PI);
  for (int i = 0; i < 7; i++) {
    for (int j = 0; j < 7; j++) {
      kernel.at<double>(i, j) =
          coeff * static_cast<double>(
                      exp(((pow(i - 3, 2) + pow(j - 3, 2)) / 2.0) * -1.0));
    }
  }
  cv::flip(kernel, kernel, -1);
  cv::filter2D(circle, circle, CV_64FC1, kernel);
  apply_gauss(&circle, 3.0);

  std::string fragment_shader_src = load("src/simple_display.shd");
  std::string ani_kuwahahra_shader_src = load("src/anisotropic_kuwahara_shader.shd");
  std::string vertex_shader_src = load("src/vertex_shader.shd");
  GLuint weights;
  glGenTextures(1, &texture);
  glGenTextures(1, &weights);

  program* filter_prog = program::make_program(vertex_shader_src, ani_kuwahahra_shader_src);
  program* simple_display_prog = program::make_program(vertex_shader_src, fragment_shader_src);

  Screen init = Screen();
  screen = &init;
  screen->init_screen(filter_prog->getProgID(), simple_display_prog->getProgID());
  screen->init_weights_map(&circle, weights);

  active_filter = true;
  if (argc > 1) {
    glutKeyboardFunc(change_image);
    glutDisplayFunc(display_images);
    input_images = std::vector<cv::Mat*>();

    // Process images
    for (size_t i = 1; argv[i] != nullptr; i++) {
      cv::Mat input = cv::imread(argv[i], cv::IMREAD_COLOR);
      cv::flip(input, input, 1);
      cv::Mat *ptr = new cv::Mat(input);
      input_images.push_back(ptr);
    }
    screen->init_texture(input_images[0], texture);
    glutMainLoop();
    for (cv::Mat *ptr : input_images) {
      delete ptr;
    }
    input_images.clear();
  } else {
    glutKeyboardFunc(activate_filter);
    glutDisplayFunc(display);
    // Use camera when no images are provided to the program
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

    // Get a pointer that stores next frame
    frame = cam.read();
    screen->init_texture(frame, texture);
    glutMainLoop();
    cam.stop();
  }
  return 0;
}