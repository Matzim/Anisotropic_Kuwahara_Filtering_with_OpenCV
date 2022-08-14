#include "utils.hh"

// Returns value of gauss function of parameter sigma at coordinate x,y
double gauss(int x, int y, double sigma) {
  double coef = 1.0 / (2.0 * CV_PI * pow(sigma, 2));
  return coef * (exp((((x * x) + (y * y)) / (2.0 * sigma * sigma)) * -1.0));
}

// Apply gauss function to all values in matrix
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

// Extract sub-matrix of size (13, 13) from matrix mat.
// Extraction is centered on matrix mat own center
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

// interpolation of coordinate x and y into the image
double bilinear_interpolation(const cv::Mat& image, double new_x,
                              double new_y) {
  double x1 = floor(new_x);
  double x2 = ceil(new_x);
  double y1 = floor(new_y);
  double y2 = ceil(new_y);

  if (x1 < 0.0) {
    x1 += 1.0;
  }
  if (y1 < 0.0) {
    y1 += 1.0;
  }
  if (x2 >= image.cols) {
    x2 -= 1.0;
  }
  if (y2 >= image.rows) {
    y2 -= 1.0;
  }
  if (x1 == x2) {
    if (y1 == y2) {
      return image.at<double>(static_cast<int>(y1), static_cast<int>(x1));
    }
    return (new_y - y1) *
               image.at<double>(static_cast<int>(y1), static_cast<int>(x1)) +
           (y2 - new_y) *
               image.at<double>(static_cast<int>(y2), static_cast<int>(x2));
  }
  if (y1 == y2) {
    return (new_x - x1) *
               image.at<double>(static_cast<int>(y1), static_cast<int>(x1)) +
           (x2 - new_x) *
               image.at<double>(static_cast<int>(y2), static_cast<int>(x2));
  }
  double dy2 = y2 - new_y;
  double dy1 = new_y - y1;
  int iy1 = static_cast<int>(y1);
  int iy2 = static_cast<int>(y2);
  int ix1 = static_cast<int>(x1);
  int ix2 = static_cast<int>(x2);
  double l1 =
      dy2 * image.at<double>(iy2, ix1) + dy1 * image.at<double>(iy1, ix1);
  double l2 =
      dy2 * image.at<double>(iy2, ix2) + dy1 * image.at<double>(iy1, ix2);
  return (x2 - new_x) * l2 + (new_x - x1) * l1;
}
