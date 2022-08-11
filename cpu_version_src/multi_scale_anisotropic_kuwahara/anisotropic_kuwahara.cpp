/*! \file anisotropic_kuwahara.cpp
    \brief Implementation of the anisotropic kuwahara filter
*/
#include "anisotropic_kuwahara.hh"

// Compute anisotropy and eigen values/vector then applies filter to rgb image
cv::Mat *kuwaharaAnisotropicFilter(cv::Mat& rgb_image) {
    cv::Mat *res = new cv::Mat();
    std::vector<cv::Mat*> structure_tensor = compute_structure_tensor(rgb_image);
    
    *res = *(structure_tensor[0]);
    
    delete structure_tensor[0];
    delete structure_tensor[1];
    delete structure_tensor[2];
    structure_tensor.clear();

    return res;
}