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

int main(int argc, char **argv) {
  // Construct masks to represent ellipse's subregions
  cv::Mat circle = cv::Mat(23, 23, CV_16S, 0.0);
  create_circle(&circle);
  std::vector<cv::Mat *> masks = get_subregions(circle);

  cv::Mat kernel = cv::Mat(13, 13, CV_64FC1);
  cv::Mat hgrid = cv::Mat(13, 13, CV_64FC1);

  for (int i = 0; i < 13; i++) {
    for (int j = 0; j < 13; j++) {
      kernel.at<double>(i, j) = static_cast<double>(i - 6.0);
      hgrid.at<double>(i, j) = static_cast<double>(j - 6.0);
    }
  }

  cv::pow(kernel, 2, kernel);
  cv::pow(hgrid, 2, hgrid);
  cv::add(kernel, hgrid, kernel);
  kernel /= 8.0;
  kernel *= -1.0;
  cv::exp(kernel, kernel);
  kernel *= (1.0 / (8.0 * CV_PI));

  if (argc > 1) {
    // Process images
    for (size_t i = 1; argv[i] != nullptr; i++) {
      cv::Mat input = cv::imread(argv[i], cv::IMREAD_COLOR);

      // Apply Anisotropic Kuwahara filter
      kuwaharaAnisotropicFilter(input, masks, kernel);

      input.convertTo(input, CV_8UC3);
      // Display resultat
      cv::imshow("Main", input);
      cv::waitKey(0);
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

  // Create Windows to display image
  cv::namedWindow("Main", cv::WINDOW_AUTOSIZE);
  cv::setWindowProperty("Main", cv::WND_PROP_FULLSCREEN, cv::WINDOW_FULLSCREEN);

  MultithreadedVideoCapture cam(&camera);
  cam.start();

  // Get a pointer that store next frame
  frame = cam.read();
  ImagePyramid pyramid = ImagePyramid(3, cv::INTER_LANCZOS4);

  while (true) {
    // Copy frame into new image
    cv::Mat image = *frame;

    // Pyramid construction is performed using Lanczos3 filter
    pyramid.build_pyramid(image);
    std::vector<cv::Mat> levels = pyramid.get_levels();

    // Apply Anisotropic Kuwahara filter
    kuwaharaAnisotropicFilter(levels[0], masks, kernel);
    levels[0].convertTo(levels[0], CV_8UC3);

    // Show live and wait for a key with timeout long enough to show images
    cv::imshow("Main", levels[0]);

    // Close "Main" window when the user press a key
    if (cv::waitKey(3) >= 0) {
      cam.stop();
      break;
    }
    pyramid.clear_pyramid();
  }
  for (int i = 0; i < NB_SUBREGIONS; i++) {
    delete masks.at(i);
  }
  return 0;
}