/*-----------------------------------------------------------------*/
/*! \file utils.hh
    \brief Functions used to create ellipses
*/
/*-----------------------------------------------------------------*/
#include "utils.hh"

/**
** Returns value of gauss function of parameter sigma at coordinate x,y
*/
double gauss(int x, int y, double sigma) {
  double coef = 1.0 / (2.0 * CV_PI * pow(sigma, 2));
  return coef * (exp((((x * x) + (y * y)) / (2.0 * sigma * sigma)) * -1.0));
}

/**
** Apply gauss function to all values in matrix
*/
void apply_gauss(cv::Mat* mask, double sigma) {
  int middle = mask->rows / 2;

  for (int i = 0; i < mask->rows; i++) {
    for (int j = 0; j < mask->cols; j++) {
      if (mask->at<double>(i, j) > 0.0) {
        mask->at<double>(i, j) *= gauss(i - middle, j - middle, sigma);
      }
    }
  }
}

/**
** Extract sub-matrix of size (13, 13) from matrix
** Extraction is centered on center of matrix
*/
cv::Mat* extractMatrix(cv::Mat* mat) {
  cv::Mat* res = new cv::Mat(13, 13, CV_64FC1);
  for (int i = 0; i < 13; i++) {
    for (int j = 0; j < 13; j++) {
      res->at<double>(i, j) = mat->at<double>(i + 5, 5 + j);
    }
  }
  delete mat;
  return res;
}