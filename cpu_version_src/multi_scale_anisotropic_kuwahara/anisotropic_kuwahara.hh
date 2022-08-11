/*! \file anisotropic_kuwahara.hh
    \brief The anisotropic Kuwahara filter is an edge-preserving filter
*/
#ifndef __ANISOTROPIC_KUWAHARA__
#define __ANISOTROPIC_KUWAHARA__

#include <opencv2/core.hpp>

#include "local_structure_estimation.hh"

// Compute anisotropy and eigen values/vector then applies filter to rgb image
cv::Mat *kuwaharaAnisotropicFilter(cv::Mat& rgb_image);

#endif // __ANISOTROPIC_KUWAHARA__