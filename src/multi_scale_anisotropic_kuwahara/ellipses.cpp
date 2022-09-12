#include "ellipses.hh"

// Get angle of coordinate x,y compare to coordinate 0,0
double atan_interval(double x, double y) {
  if (x > 0) {
    if (y >= 0) {
      return atan(y / x);
    } else {
      return atan(y / x) + 2.0 * CV_PI;
    }
  } else if (x < 0) {
    return atan(y / x) + CV_PI;
  } else {
    if (y > 0) {
      return (CV_PI / 2.0);
    } else if (y < 0) {
      return (3.0 * CV_PI) / 2.0;
    }
  }
  return -1.0;
}

// Create ellipse with a certain number of circular subregions
void create_circle(cv::Mat* matrix) {
  int radius = matrix->rows / 2;

  double d_pi = 2.0 * CV_PI;
  for (int i = 0; i < NB_SUBREGIONS; i++) {
    double lower_bound = (((2 * i - 1) * CV_PI) / NB_SUBREGIONS);
    double upper_bound = (((2 * i + 1) * CV_PI) / NB_SUBREGIONS);
    bool overflow = lower_bound < 0;

    for (int x = 0; x < matrix->rows; x++) {
      for (int y = 0; y < matrix->cols; y++) {
        double angle = atan_interval(x - radius, y - radius);

        if ((lower_bound <= angle ||
             (overflow && lower_bound + d_pi <= angle)) &&
            (angle < upper_bound || (overflow && angle < upper_bound + d_pi))) {
          matrix->at<short>(x, y) = static_cast<short>(i + 1);
        }
      }
    }
  }
}

// Only keep values where distance from matrix center is below or equal to h
void cut_circle(cv::Mat* matrix, int h) {
  int radius = matrix->rows / 2;
  for (int i = 0; i < NB_SUBREGIONS; i++) {
    for (int x = 0; x < matrix->rows; x++) {
      for (int y = 0; y < matrix->cols; y++) {
        if (sqrt(pow(x - radius, 2) + pow(y - radius, 2)) > h) {
          matrix->at<double>(x, y) = 0.0;
        }
      }
    }
  }
}

// Create all subregions (one for each direction) from matrix circle
std::vector<cv::Mat*> get_subregions(cv::Mat& circle) {
  std::vector<cv::Mat*> masks = std::vector<cv::Mat*>();
  int middle = circle.cols / 2;

  for (int i = 0; i < NB_SUBREGIONS; i++) {
    cv::Mat* mask = new cv::Mat(circle.rows, circle.cols, CV_64FC1, 0.0);
    mask->at<double>(middle, middle) = static_cast<double>(NB_SUBREGIONS);
    masks.emplace_back(mask);
  }

  for (int x = 0; x < circle.rows; x++) {
    for (int y = 0; y < circle.cols; y++) {
      short val = circle.at<short>(x, y);
      if (val > 0) {
        masks[val - 1]->at<double>(x, y) = static_cast<double>(NB_SUBREGIONS);
      }
    }
  }
  cv::Mat kernel = cv::Mat(7, 7, CV_64FC1, 0.0);
  double coeff = 1.0 / (2.0 * CV_PI);
  for (int i = 0; i < 7; i++) {
    for (int j = 0; j < 7; j++) {
      kernel.at<double>(i, j) =
          coeff * static_cast<double>(
                      exp(((pow(i - 3, 2) + pow(j - 3, 2)) / 2.0) * -1.0));
    }
  }
  cv::flip(kernel, kernel, -1);

  for (int i = 0; i < NB_SUBREGIONS; i++) {
    cv::filter2D(*(masks[i]), *(masks[i]), CV_64FC1, kernel);
    apply_gauss(masks[i], 3.0);
    cut_circle(masks[i], 6);
    masks[i] = extractMatrix(masks[i]);
  }
  return masks;
}