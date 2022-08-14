/*! \file anisotropic_kuwahara.cpp
    \brief Implementation of the anisotropic kuwahara filter
*/
#include "anisotropic_kuwahara.hh"

// Applies an anisotropic kuwahara filter on greyscale image,
void _kuwaharaAnisotropicFilterGrey(cv::Mat &channel,
                                    std::vector<cv::Mat *> masks,
                                    const cv::Mat &anisotropy,
                                    const cv::Mat &local_orientation) {
  channel.convertTo(channel, CV_64FC1);
  std::vector<cv::Mat *> mis = std::vector<cv::Mat *>();
  std::vector<cv::Mat *> sis = std::vector<cv::Mat *>();

  cv::Mat channel_pow2;
  cv::pow(channel, 2, channel_pow2);

  // Compute mi and si coefficients
  for (int x = 0; x < NB_SUBREGIONS; x++) {
    cv::Mat *mi = new cv::Mat(channel.rows, channel.cols, CV_64FC1, 0.0);
    cv::Mat *si = new cv::Mat(channel.rows, channel.cols, CV_64FC1, 0.0);
    for (int i = 0; i < channel.rows; i++) {
      for (int j = 0; j < channel.cols; j++) {
        double sum_mi = 0.0;
        double sum_si = 0.0;
        cv::Mat *mask = masks.at(x);
        double angle = local_orientation.at<double>(i, j);
        for (int mx = 0; mx < mask->rows; mx++) {
          for (int mj = 0; mj < mask->cols; mj++) {
            double weight = mask->at<double>(mx, mj);
            if (weight != 0.0) {
              double iso = 1.0 + anisotropy.at<double>(i, j);
              double temp_j = static_cast<double>((mj - 6)) * (1.0 / iso);
              double temp_i = static_cast<double>((mx - 6)) * iso;
              double cos_angle = cos(angle);
              double sin_angle = sin(angle);
              double new_x = (cos_angle * temp_j - sin_angle * temp_i) +
                             static_cast<double>(j);
              double new_y = (sin_angle * temp_j + cos_angle * temp_i) +
                             static_cast<double>(i);
              if (new_x >= 0.0 && new_x < channel.cols && new_y >= 0.0 &&
                  new_y < channel.rows) {
                sum_mi +=
                    weight * bilinear_interpolation(channel, new_x, new_y);
                sum_si +=
                    weight * bilinear_interpolation(channel_pow2, new_x, new_y);
              }
            }
          }
        }
        si->at<double>(i, j) = sum_si;
        mi->at<double>(i, j) = sum_mi;
      }
    }
    mis.emplace_back(mi);
    cv::Mat tmp;
    cv::pow(*mi, 2, tmp);
    *si -= tmp;
    for (int i = 0; i < si->rows * si->cols; i++) {
      if (si->at<double>(i) < 0.0) {
        si->at<double>(i) *= -1.0;
      }
    }
    cv::sqrt(*si, *si);
    sis.emplace_back(si);
  }

  // Compute final image
  for (int i = 0; i < channel.rows * channel.cols; i++) {
    double sum_alpha = 0.0;
    double sum_mi = 0.0;
    for (int x = 0; x < NB_SUBREGIONS; x++) {
      double alpha =
          1.0 / (1.0 + pow(sis.at(x)->at<double>(i), SHARPENESS_PARAMETER));
      sum_alpha += alpha;
      sum_mi += alpha * mis.at(x)->at<double>(i);
    }
    channel.at<double>(i) = sum_mi / sum_alpha;
  }
  for (size_t i = 0; i < mis.size(); i++) {
    delete mis.at(i);
    delete sis.at(i);
  }
}

// Compute anisotropy and eigen values/vector then applies filter to rgb image
cv::Mat *kuwaharaAnisotropicFilter(cv::Mat &rgb_image,
                                   std::vector<cv::Mat *> masks) {
  cv::Mat channels[3];
  cv::split(rgb_image, channels);

  cv::Mat gray;
  cv::cvtColor(rgb_image, gray, cv::COLOR_BGR2GRAY);

  std::vector<cv::Mat *> structure_tensor = compute_structure_tensor(gray);
  std::vector<cv::Mat *> eigen_values = compute_eigen_values(
      structure_tensor[0], structure_tensor[1], structure_tensor[2]);

  // GOOD ! Results differ with the grayscale

  // Compute the local orientation in the direction of the minor eigenvector
  cv::Mat f2 = structure_tensor[1]->mul(2.0);
  cv::Mat local_orientation;
  cv::subtract(*(structure_tensor[0]), *(structure_tensor[2]),
               local_orientation, cv::noArray(), CV_64FC1);

  for (int i = 0; i < local_orientation.rows; i++) {
    for (int j = 0; j < local_orientation.cols; j++) {
      local_orientation.at<double>(i, j) = static_cast<double>(
          std::atan2(local_orientation.at<double>(i, j), f2.at<double>(i, j)));
    }
  }
  local_orientation *= 0.5;
  local_orientation += (CV_PI / 2.0);
  local_orientation *= -1.0;

  // Compute the anisotropy of the image
  cv::Mat anisotropy;
  cv::Mat tmp;
  cv::subtract(*(eigen_values[0]), *(eigen_values[1]), anisotropy,
               cv::noArray(), CV_64FC1);
  cv::add(*(eigen_values[0]), *(eigen_values[1]), tmp, cv::noArray(), CV_64FC1);
  anisotropy /= tmp;

  _kuwaharaAnisotropicFilterGrey(channels[0], masks, anisotropy,
                                 local_orientation);
  _kuwaharaAnisotropicFilterGrey(channels[1], masks, anisotropy,
                                 local_orientation);
  _kuwaharaAnisotropicFilterGrey(channels[2], masks, anisotropy,
                                 local_orientation);

  cv::Mat *res = new cv::Mat();
  cv::merge(channels, 3, *res);

  delete structure_tensor[0];
  delete structure_tensor[1];
  delete structure_tensor[2];
  delete eigen_values[0];
  delete eigen_values[1];

  return res;
}