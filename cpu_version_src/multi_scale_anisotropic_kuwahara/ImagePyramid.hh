/*! \file ImagePyramid.hh
    \brief Low-pass filtered pyramid
*/
#ifndef __IMAGE_PYRAMID__
#define __IMAGE_PYRAMID__
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <vector>

class ImagePyramid {
 public:
  ImagePyramid();
  ImagePyramid(const int nb_level, cv::InterpolationFlags flag);

  void build_pyramid(cv::Mat& image);
  void clear_pyramid();

  std::vector<cv::Mat>& get_levels();

 private:
  const double scale_factor_horizontal = 0.5;
  const double scale_factor_vertical = 0.5;
  int nb_level_;
  cv::InterpolationFlags interpolation_flag_;
  std::vector<cv::Mat> pyramid_levels_;
};

#endif  // __IMAGE_PYRAMID__