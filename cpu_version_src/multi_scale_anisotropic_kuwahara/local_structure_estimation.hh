/*! \file local_structure_estimation.hh
    \brief The local orientation and anisotropy estimation is based
    on the eigenvalues and eigenvectors of the structure tensor

    Calculate the structure tensor directly from the RGB values of the input
*/
#ifndef __LOCAL_STRUCTURE_ESTIMATION__
#define __LOCAL_STRUCTURE_ESTIMATION__

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <vector>

/*
** Calculate approximations of the partial derivative
*/
std::vector<cv::Mat*> gauss_derivatives(cv::Mat& img);

/*
** Calculate the structure tensor of the image
*/
std::vector<cv::Mat*> compute_structure_tensor(cv::Mat& channels, const cv::Mat &kernel);

// compute the eigen values of the image
std::vector<cv::Mat*> compute_eigen_values(cv::Mat* E, cv::Mat* F, cv::Mat* G);

#endif  // __LOCAL_STRUCTURE_ESTIMATION__