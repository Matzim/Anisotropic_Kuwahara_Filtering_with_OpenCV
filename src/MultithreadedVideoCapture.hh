/*! \file MultithreadedVideoCapture.hh
    \brief Video capture that use a detached thread
    to collect frames and avoid blocking I/O operations
*/
#ifndef __MULTITHREADED_VIDEO_CAPTURE__
#define __MULTITHREADED_VIDEO_CAPTURE__

#include <iostream>
#include <mutex>
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <thread>

class MultithreadedVideoCapture {
 public:
  MultithreadedVideoCapture(){};
  MultithreadedVideoCapture(cv::VideoCapture* cam)
      : camera_(cam), stop_(false) {
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
  std::mutex mutex_;
};

#endif  // __MULTITHREADED_VIDEO_CAPTURE__