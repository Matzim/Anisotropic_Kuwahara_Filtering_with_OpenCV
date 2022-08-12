/*! \file anisotropic_kuwahara.cpp
    \brief Implementation of the anisotropic kuwahara filter
*/
#include "anisotropic_kuwahara.hh"

// Applies an anisotropic kuwahara filter on greyscale image,
cv::Mat *_kuwaharaAnisotropicFilterGrey() {
    std::vector<cv::Mat*> mis = std::vector<cv::Mat*>();
    std::vector<cv::Mat*> sis = std::vector<cv::Mat*>();
}

// Compute anisotropy and eigen values/vector then applies filter to rgb image
cv::Mat *kuwaharaAnisotropicFilter(cv::Mat& rgb_image, std::vector<cv::Mat *> masks) {
    cv::Mat channels[3];
    cv::split(rgb_image, channels);

    std::vector<cv::Mat*> structure_tensor = compute_structure_tensor(channels);
    std::vector<cv::Mat*> eigen_values = compute_eigen_values(structure_tensor[0], structure_tensor[1], structure_tensor[2]);

    // Compute the local orientation in the direction of the minor eigenvector
    cv::Mat numerator = *structure_tensor[1] * 2.0;
    cv::Mat denom = *structure_tensor[0] - *structure_tensor[2];
    
    /*
    local_orientation *= 0.5;
    local_orientation += CV_PI / 2.0;
    */
    // Compute the anisotropy of the image
    cv::Mat anisotropy = (*(eigen_values[0]) - *(eigen_values[1])) / (*(eigen_values[0]) + *(eigen_values[1]));
    
//    delete structure_tensor[0];
    delete structure_tensor[1];
    delete structure_tensor[2];
    delete eigen_values[0];
    delete eigen_values[1];

    return structure_tensor[0];
}