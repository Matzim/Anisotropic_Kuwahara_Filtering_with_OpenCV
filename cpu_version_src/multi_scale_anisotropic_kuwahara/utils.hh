/*! \file utils.hh
    \brief Functions
*/
#ifndef __UTILS__
#define __UTILS__

#define STANDARD_DEVIATION 3

#include <opencv2/core.hpp>

// Apply gauss function to all values in matrix
void apply_gauss(cv::Mat* mask, double coeff);

cv::Mat* extractMatrix(cv::Mat* mat);

// interpolation of coordinate x and y into the image
double bilinear_interpolation(const cv::Mat& image, double new_x, double new_y);

#endif  // __UTILS__