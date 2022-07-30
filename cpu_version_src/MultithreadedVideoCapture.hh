#ifndef __MULTITHREADED_VIDEO_CAPTURE__
#define __MULTITHREADED_VIDEO_CAPTURE__

#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <mutex>
#include <thread>
#include <iostream>


class MultithreadedVideoCapture {
    public:
    MultithreadedVideoCapture() {};
    MultithreadedVideoCapture(cv::VideoCapture* cam) : camera_(cam), stop_(false) {
        this->camera_->read(this->frame_);
    };

    void start();
    void get_next_frame();
    cv::Mat* read();
    void stop();

    private:
    cv::VideoCapture* camera_;
    cv::Mat frame_;
    bool stop_;
    std::mutex mutex;
};

#endif // __MULTITHREADED_VIDEO_CAPTURE__