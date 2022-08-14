/*! \file ellipses.hh
    \brief Functions used to create ellipses used in the anisotropic Kuwahara
   filter
*/
#ifndef __ELLIPSES__
#define __ELLIPSES__

// Parameters of the Anisotropic Kuwahara filter
#define NB_SUBREGIONS 8
#define SHARPENESS_PARAMETER 8

#include <cmath>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <vector>

#include "utils.hh"

// Create ellipse with a certain number of circular subregions
void create_circle(cv::Mat *matrix);

// Only keep values where distance from matrix center is below or equal to h
void cut_circle(cv::Mat *mat, int h);

// Create all subregions (one for each direction) from matrix circle
std::vector<cv::Mat *> get_subregions(cv::Mat &circle);

#endif  // __ELLIPSES__