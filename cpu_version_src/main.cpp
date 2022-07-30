#include "MultithreadedVideoCapture.hh"

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

    while (true)
    {
        // Wait for a new frame from camera and store it into 'frame'
        frame = cam.read();

        // Show live and wait for a key with timeout long enough to show images
        cv::imshow("Main", *frame);

        // Close "Main" window when the user press a key !
        if (cv::waitKey(5) >= 0) {
            cam.stop();
            break;
        }
    }
    return 0;
}