#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <stdio.h>

int main(int, char**)
{
    cv::Mat frame;
    // Initialize videocapture
    cv::VideoCapture camera;

    int deviceID = 0;             // 0 = open default camera
    int apiID = cv::CAP_ANY;      // 0 = autodetect default API

    // open selected camera
    camera.open(deviceID, apiID);
    // check if we succeeded
    if (!camera.isOpened()) {
        std::cerr << "ERROR ! Unable to open camera !\n";
        return -1;
    }
    camera.set(cv::CAP_PROP_FRAME_WIDTH, 1920);
    camera.set(cv::CAP_PROP_FRAME_HEIGHT, 1080);

    std::cout << "You are using " << camera.getBackendName()
    << "\nPress any key to stop video capture." << std::endl;
    cv::namedWindow("Main", cv::WINDOW_AUTOSIZE);
    for (;;)
    {
        // wait for a new frame from camera and store it into 'frame'
        camera.read(frame);

        // check if we succeeded
        if (frame.empty()) {
            std::cerr << "ERROR! blank frame grabbed\n";
            break;
        }
        // show live and wait for a key with timeout long enough to show images
        cv::imshow("Main", frame);

        if (cv::waitKey(5) >= 0 || cv::getWindowProperty("Main", cv::WND_PROP_AUTOSIZE) == -1) {
            break;
        }
    }
    return 0;
}