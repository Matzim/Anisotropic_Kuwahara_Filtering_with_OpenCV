/*! \file utils.hh
    \brief Functions
*/
#ifndef __UTILS__
#define __UTILS__

#include <opencv2/core.hpp>

// Apply gauss function to all values in matrix
void apply_gauss(cv::Mat* mask, double coeff);

cv::Mat* extractMatrix(cv::Mat* mat);

#endif  // __UTILS__