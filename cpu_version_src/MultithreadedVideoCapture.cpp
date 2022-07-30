#include "MultithreadedVideoCapture.hh"

void MultithreadedVideoCapture::start() {
    std::thread thread(&MultithreadedVideoCapture::get_next_frame, this);
    thread.detach();
}

void MultithreadedVideoCapture::get_next_frame() {
    while (true) {
        if (this->stop_)
            return;

        this->mutex.lock();
        this->camera_->read(this->frame_);
        if (this->frame_.empty() && !this->stop_) {
            std::cerr << "Error: VideoCapture retrieves a blank frame !" << std::endl;
            return;
        }
        this->mutex.unlock();
    }
}

cv::Mat* MultithreadedVideoCapture::read() {
    return &this->frame_;
}

void MultithreadedVideoCapture::stop() {
    this->stop_ = true;
}