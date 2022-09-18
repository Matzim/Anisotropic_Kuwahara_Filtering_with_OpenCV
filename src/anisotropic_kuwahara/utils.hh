/*-----------------------------------------------------------------*/
/*! \file utils.hh
    \brief Functions used to create ellipses
*/
/*-----------------------------------------------------------------*/
#ifndef __UTILS__
#define __UTILS__

#include <opencv2/core.hpp>

// Apply gauss function to all values in matrix
void apply_gauss(cv::Mat* mask, double sigma);

// Extract sub-matrix of size (13, 13) from matrix
// Extraction is centered on center of matrix
cv::Mat* extractMatrix(cv::Mat* mat);

#endif  // __UTILS__