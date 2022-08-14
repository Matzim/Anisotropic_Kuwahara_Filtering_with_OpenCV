/*! \file main.cpp
    \brief Main fonction of the program
*/

#include <ctime>
#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>

#include "MultithreadedVideoCapture.hh"
#include "multi_scale_anisotropic_kuwahara/ImagePyramid.hh"
#include "multi_scale_anisotropic_kuwahara/anisotropic_kuwahara.hh"
#include "multi_scale_anisotropic_kuwahara/ellipses.hh"

/**
    Main function
*/
int main(int argc, char **argv) {
  // Construct masks to represent ellipse's subregions
  cv::Mat circle = cv::Mat(23, 23, CV_16S, 0.0);
  create_circle(&circle);
  std::vector<cv::Mat *> masks = get_subregions(circle);

  if (argc > 1) {
    // Process images
    for (size_t i = 1; argv[i] != nullptr; i++) {
      cv::Mat input = cv::imread(argv[i], cv::IMREAD_COLOR);

      // Apply Anisotropic Kuwahara filter
      cv::Mat *res = kuwaharaAnisotropicFilter(input, masks);
      cv::Mat B;
      res->convertTo(B, CV_8UC3);
      // Display resultat !
      cv::imshow("Main", B);
      if (cv::waitKey(0) >= 0) {
          break;
      }
    }
    for (int i = 0; i < NB_SUBREGIONS; i++) {
      delete masks.at(i);
    }
    return 0;
  }

  // Use camera when no arguments are provided to the program
  cv::Mat *frame;
  cv::VideoCapture camera;  // Initialize VideoCapture

  // Open default camera and autodetect default API
  camera.open(0, cv::CAP_ANY);

  // Check if we succeeded
  if (!camera.isOpened()) {
    std::cerr << "ERROR ! Unable to open camera !" << std::endl;
    return -1;
  }
  camera.set(cv::CAP_PROP_FRAME_WIDTH, 512);
  camera.set(cv::CAP_PROP_FRAME_HEIGHT, 512);

  std::cout << "You are using " << camera.getBackendName()
            << "\nPress any key to stop video capture." << std::endl;
  cv::namedWindow("Main", cv::WINDOW_AUTOSIZE);
  cv::setWindowProperty("Main", cv::WND_PROP_FULLSCREEN, cv::WINDOW_FULLSCREEN);

  MultithreadedVideoCapture cam(&camera);
  cam.start();

  // Get a pointer that store next camera frame
  frame = cam.read();
  ImagePyramid pyramid = ImagePyramid(3, cv::INTER_LANCZOS4);

  while (true) {
    // Copy frame into new image
    cv::Mat image = *frame;

    // Pyramid construction is performed using Lanczos3 filter
    pyramid.build_pyramid(image);
    std::vector<cv::Mat> levels = pyramid.get_levels();

    // Apply Anisotropic Kuwahara filter
    cv::Mat *res = kuwaharaAnisotropicFilter(levels[0], masks);

    cv::Mat B;
    res->convertTo(B, CV_8U);
    // Show live and wait for a key with timeout long enough to show images
    cv::imshow("Main", B);

    // Close "Main" window when the user press a key !
    if (cv::waitKey(5) >= 0) {
      cam.stop();
      break;
    }
    delete res;
    pyramid.clear_pyramid();
  }
  for (int i = 0; i < NB_SUBREGIONS; i++) {
    delete masks.at(i);
  }

  return 0;
}