#include "MultithreadedVideoCapture.hh"
#include "multi_scale_anisotropic_kuwahara/ImagePyramid.hh"

#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <ctime>

int main(int, char**)
{
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
/*
    cv::Mat channels[3];
    cv::split(*frame, channels);
        cv::Mat blue_channel = channels[0];
        cv::Mat red_channel = channels[1];
        cv::Mat green_channel = channels[2];
*/
    ImagePyramid pyramid = ImagePyramid(3, cv::INTER_LANCZOS4);
    while (true)
    {
        // Copy frame into new image
        cv::Mat image = *frame;
        pyramid.build_pyramid(image);
        const std::vector<cv::Mat> levels = pyramid.get_levels();

        // Show live and wait for a key with timeout long enough to show images
        cv::imshow("Main", levels[0]);

        // Close "Main" window when the user press a key !
        if (cv::waitKey(5) >= 0) {
            cam.stop();
            break;
        }
        pyramid.clear_pyramid();
    }
    return 0;
}