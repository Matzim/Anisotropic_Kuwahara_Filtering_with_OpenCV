/*-----------------------------------------------------------------*/
/*! \file MultithreadedVideoCapture.cpp
    \brief Implementation of the video capture that use a detached thread
    to collect frames and avoid blocking I/O operations
*/
/*-----------------------------------------------------------------*/
#include "MultithreadedVideoCapture.hh"

void MultithreadedVideoCapture::start() {
  std::thread thread(&MultithreadedVideoCapture::get_next_frame, this);
  thread.detach();
}

cv::Mat* MultithreadedVideoCapture::read() { return &this->frame_; }

void MultithreadedVideoCapture::stop() { this->stop_ = true; }

void MultithreadedVideoCapture::get_next_frame() {
  while (true) {
    if (this->stop_) return;

    this->mutex_.lock();
    this->camera_->read(this->frame_);
    if (this->frame_.empty() && !this->stop_) {
      std::cerr << "Error: VideoCapture retrieves a blank frame !" << std::endl;
      return;
    }
    this->mutex_.unlock();
  }
}