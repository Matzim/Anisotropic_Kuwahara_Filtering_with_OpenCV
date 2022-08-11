/*! \file ImagePyramid.hh
    \brief Implemantation of the construction of the low-pass filtered pyramid
*/
#include "ImagePyramid.hh"

ImagePyramid::ImagePyramid() : nb_level_(3), interpolation_flag_(cv::INTER_CUBIC) {
    this->pyramid_levels_ = std::vector<cv::Mat>();
};

ImagePyramid::ImagePyramid(const int nb_level, cv::InterpolationFlags interpolation)
: nb_level_(nb_level), interpolation_flag_(interpolation) {
    this->pyramid_levels_ = std::vector<cv::Mat>();
};

void ImagePyramid::build_pyramid(cv::Mat& image) {
    this->pyramid_levels_.push_back(image);
    for (int i = 0; i < this->nb_level_; i++) {
        int rows = image.rows * this->scale_factor_vertical;
        int cols = image.cols * this->scale_factor_horizontal;
        cv::resize(
            image, image, cv::Size(cols, rows), this->scale_factor_horizontal,
            this->scale_factor_vertical, this->interpolation_flag_
        );
        this->pyramid_levels_.push_back(image);
    }
};

void ImagePyramid::clear_pyramid() {
    this->pyramid_levels_.clear();
};

std::vector<cv::Mat>& ImagePyramid::get_levels() {
    return this->pyramid_levels_;
};