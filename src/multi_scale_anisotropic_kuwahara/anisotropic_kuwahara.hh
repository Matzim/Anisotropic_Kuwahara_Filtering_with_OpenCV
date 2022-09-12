/*! \file anisotropic_kuwahara.hh
    \brief The anisotropic Kuwahara filter is an edge-preserving filter
*/
#ifndef __ANISOTROPIC_KUWAHARA__
#define __ANISOTROPIC_KUWAHARA__

#include <algorithm>
#include <opencv2/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "ellipses.hh"
#include "local_structure_estimation.hh"

// Compute anisotropy and eigen values/vector then applies filter to rgb image
// cv::Mat *kuwaharaAnisotropicFilter(cv::Mat &rgb_image, std::vector<cv::Mat *>
// masks, const cv::Mat &kernel);
void kuwaharaAnisotropicFilter(cv::Mat &rgb_image, std::vector<cv::Mat *> masks,
                               const cv::Mat &kernel);

#endif  // __ANISOTROPIC_KUWAHARA__