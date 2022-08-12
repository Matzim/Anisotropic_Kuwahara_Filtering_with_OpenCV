/*! \file main.cpp
    \brief Main fonction of the program
*/

#include "MultithreadedVideoCapture.hh"
#include "multi_scale_anisotropic_kuwahara/ImagePyramid.hh"
#include "multi_scale_anisotropic_kuwahara/anisotropic_kuwahara.hh"
#include "multi_scale_anisotropic_kuwahara/ellipses.hh"

#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <ctime>

/**
    Main function
*/
int main(int argc, char** argv)
{
    if (argc > 1) {
        // Process images
        for (size_t i = 1; argv[i] != nullptr; i++) {
            std::cout << argv[i] << std::endl;
        }
        return 0;
    }

    // Use camera when no arguments are provided to the program
    cv::Mat *frame;
    cv::VideoCapture camera;    // Initialize VideoCapture

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

    // Construct masks to represent ellipse's subregions
    cv::Mat circle = cv::Mat(23, 23, CV_16S);
    create_circle(&circle);

    std::vector<cv::Mat*> masks = get_subregions(circle);
    for (int index = 0; index < 8; index++) {
        std::cout << masks[index]->rows << " " << masks[index]->cols << std::endl;
        for (int i = 0; i < masks[index]->rows; i++) {
            for (int j = 0; j < masks[index]->cols; j++) {
                std::cout << masks[index]->at<double>(i, j) << " |";
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }

    while (true)
    {
        // Copy frame into new image
        cv::Mat image = *frame;
        
        // Pyramid construction is performed using Lanczos3 filter
        pyramid.build_pyramid(image);
        std::vector<cv::Mat> levels = pyramid.get_levels();
        // cv::imshow("Main", levels[0]);

        // Apply Anisotropic Kuwahara filter
        cv::Mat *res = kuwaharaAnisotropicFilter(levels[0], masks);
        cv::Mat B;
        res->convertTo(B, CV_8U);
        cv::imshow("Main", B);
        // Show live and wait for a key with timeout long enough to show images
        //cv::imshow("Main", *res);
        
        // Close "Main" window when the user press a key !
        if (cv::waitKey(5) >= 0) {
            cam.stop();
            break;
        }
        pyramid.clear_pyramid();
    }
    return 0;
}