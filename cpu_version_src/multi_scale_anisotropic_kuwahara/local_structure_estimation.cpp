/*! \file local_structure_estimation.cpp
    \brief Implementation of the local structure estimation
*/
#include "local_structure_estimation.hh"

// Return x and y gaussian derivatives so it can be convolved with the image
std::vector<cv::Mat *> gauss_derivative_kernel(const cv::Mat &kernel) {
  cv::Mat mgrid = cv::Mat(13, 13, CV_64FC1);
  cv::Mat hgrid = cv::Mat(13, 13, CV_64FC1);

  for (int i = 0; i < 13; i++) {
    for (int j = 0; j < 13; j++) {
      mgrid.at<double>(i, j) = static_cast<double>(i - 6.0);
      hgrid.at<double>(i, +j) = static_cast<double>(j - 6.0);
    }
  }
  cv::Mat *Dx = new cv::Mat();
  cv::Mat *Dy = new cv::Mat();
  *Dx = hgrid.mul(kernel);
  *Dy = mgrid.mul(kernel);
  *Dx *= -1;
  *Dy *= -1;
  return std::vector<cv::Mat *>({Dx, Dy});
}

std::vector<cv::Mat *> gauss_derivatives(cv::Mat &img, const cv::Mat &kernel) {
  cv::Mat *partial_derivative_x = new cv::Mat();
  cv::Mat *partial_derivative_y = new cv::Mat();

  std::vector<cv::Mat *> vect = gauss_derivative_kernel(kernel);
  cv::flip(*(vect[0]), *(vect[0]), -1);
  cv::flip(*(vect[1]), *(vect[1]), -1);

  cv::filter2D(img, *partial_derivative_x, CV_64FC1, *(vect[0]));
  cv::filter2D(img, *partial_derivative_y, CV_64FC1, *(vect[1]));
  delete vect[0];
  delete vect[1];
  return std::vector<cv::Mat *>({partial_derivative_x, partial_derivative_y});
}

std::vector<cv::Mat *> compute_structure_tensor(cv::Mat &gray, const cv::Mat &kernel) {
  // Calculate approximations of the partial derivative
  std::vector<cv::Mat *> deriv = gauss_derivatives(gray, kernel);

  cv::Mat tmpE = cv::Mat(gray.rows, gray.cols, CV_64FC1);
  cv::Mat tmpF = cv::Mat(gray.rows, gray.cols, CV_64FC1);
  cv::Mat tmpG = cv::Mat(gray.rows, gray.cols, CV_64FC1);

  cv::pow(*deriv[0], 2, tmpE);
  tmpF = (deriv[0])->mul(*deriv[1]);
  cv::pow(*deriv[1], 2, tmpG);

  cv::Mat *E = new cv::Mat(gray.rows, gray.cols, CV_64FC1);
  cv::Mat *F = new cv::Mat(gray.rows, gray.cols, CV_64FC1);
  cv::Mat *G = new cv::Mat(gray.rows, gray.cols, CV_64FC1);

  cv::flip(kernel, kernel, -1);

  cv::filter2D(tmpE, *E, CV_64FC1, kernel);
  cv::filter2D(tmpF, *F, CV_64FC1, kernel);
  cv::filter2D(tmpG, *G, CV_64FC1, kernel);

  delete deriv[0];
  delete deriv[1];
  return std::vector<cv::Mat *>({E, F, G});
}

std::vector<cv::Mat *> compute_eigen_values(cv::Mat *E, cv::Mat *F,
                                            cv::Mat *G) {
  cv::Mat EG;
  cv::add(*E, *G, EG, cv::noArray(), CV_64FC1);
  cv::Mat sub;
  cv::Mat sqrt = cv::Mat(F->rows, F->cols, CV_64FC1);

  cv::subtract(*E, *G, sub, cv::noArray(), CV_64FC1);
  cv::pow(sub, 2, sub);
  cv::pow(*F, 2.0, sqrt);
  sqrt *= 4.0;
  cv::add(sqrt, sub, sqrt, cv::noArray(), CV_64FC1);
  cv::sqrt(sqrt, sqrt);

  cv::Mat *l1 = new cv::Mat(F->rows, F->cols, CV_64FC1, 0.0);
  cv::Mat *l2 = new cv::Mat(F->rows, F->cols, CV_64FC1, 0.0);

  cv::add(EG, sqrt, *l1, cv::noArray(), CV_64FC1);
  cv::subtract(EG, sqrt, *l2, cv::noArray(), CV_64FC1);
  *l1 /= 2.0;
  *l2 /= 2.0;

  return std::vector<cv::Mat *>({l1, l2});
}