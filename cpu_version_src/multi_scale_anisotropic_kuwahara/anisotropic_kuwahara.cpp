/*! \file anisotropic_kuwahara.cpp
    \brief Implementation of the anisotropic kuwahara filter
*/
#include "anisotropic_kuwahara.hh"

// Applies an anisotropic kuwahara filter on greyscale image,
void _kuwaharaAnisotropicFilterGrey(cv::Mat *channels,
                                    std::vector<cv::Mat *> masks,
                                    const cv::Mat &anisotropy,
                                    const cv::Mat &local_orientation) {
  for (int i = 0; i < 3; i++) {
    channels[i].convertTo(channels[i], CV_64FC1);
  }

  std::vector<cv::Mat *> mis = std::vector<cv::Mat *>();
  std::vector<cv::Mat *> sis = std::vector<cv::Mat *>();

  cv::Mat channel0_pow2;
  cv::Mat channel1_pow2;
  cv::Mat channel2_pow2;
  cv::pow(channels[0], 2, channel0_pow2);
  cv::pow(channels[1], 2, channel1_pow2);
  cv::pow(channels[2], 2, channel2_pow2);

  int rows = channels[0].rows;
  int cols = channels[0].cols;

  // Compute mi and si coefficients
  for (int x = 0; x < NB_SUBREGIONS; x++) {
    cv::Mat *mi0 = new cv::Mat(rows, cols, CV_64FC1);
    cv::Mat *si0 = new cv::Mat(rows, cols, CV_64FC1);
    cv::Mat *mi1 = new cv::Mat(rows, cols, CV_64FC1);
    cv::Mat *si1 = new cv::Mat(rows, cols, CV_64FC1);
    cv::Mat *mi2 = new cv::Mat(rows, cols, CV_64FC1);
    cv::Mat *si2 = new cv::Mat(rows, cols, CV_64FC1);
    for (int i = 0; i < rows; i++) {
      for (int j = 0; j < cols; j++) {
        double sum_mi_channel0 = 0.0;
        double sum_si_channel0 = 0.0;
        double sum_mi_channel1 = 0.0;
        double sum_si_channel1 = 0.0;
        double sum_mi_channel2 = 0.0;
        double sum_si_channel2 = 0.0;
        cv::Mat *mask = masks.at(x);
        double angle = local_orientation.at<double>(i, j);
        for (int mx = 0; mx < 13; mx++) {
          for (int mj = 0; mj < 13; mj++) {
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
              if (new_x >= 0.0 && new_x < cols && new_y >= 0.0 &&
                  new_y < rows) {
                sum_mi_channel0 +=
                    weight * bilinear_interpolation(channels[0], new_x, new_y);
                sum_si_channel0 += weight * bilinear_interpolation(
                                                channel0_pow2, new_x, new_y);
                sum_mi_channel1 +=
                    weight * bilinear_interpolation(channels[1], new_x, new_y);
                sum_si_channel1 += weight * bilinear_interpolation(
                                                channel1_pow2, new_x, new_y);
                sum_mi_channel2 +=
                    weight * bilinear_interpolation(channels[2], new_x, new_y);
                sum_si_channel2 += weight * bilinear_interpolation(
                                                channel2_pow2, new_x, new_y);
              }
            }
          }
        }
        mi0->at<double>(i, j) = sum_mi_channel0;
        si0->at<double>(i, j) = sum_si_channel0;
        mi1->at<double>(i, j) = sum_mi_channel1;
        si1->at<double>(i, j) = sum_si_channel1;
        mi2->at<double>(i, j) = sum_mi_channel2;
        si2->at<double>(i, j) = sum_si_channel2;
      }
    }
    mis.emplace_back(mi0);
    mis.emplace_back(mi1);
    mis.emplace_back(mi2);
    sis.emplace_back(si0);
    sis.emplace_back(si1);
    sis.emplace_back(si2);
  }

  for (size_t i = 0; i < mis.size(); i++) {
    cv::Mat tmp;
    cv::pow(*(mis[i]), 2, tmp);
    *(sis[i]) -= tmp;
    for (int x = 0; x < rows * cols; x++) {
      if (sis[i]->at<double>(x) < 0.0) {
        sis[i]->at<double>(x) *= -1.0;
      }
    }
    cv::sqrt(*(sis[i]), *(sis[i]));
    cv::pow(*(sis[i]), SHARPENESS_PARAMETER, *(sis[i]));
    *(sis[i]) += 1.0;
    *(sis[i]) = (1.0 / *(sis[i]));
  }
  cv::Mat sum_alpha0 = cv::Mat(rows, cols, CV_64FC1, 0.0);
  cv::Mat sum_alpha1 = cv::Mat(rows, cols, CV_64FC1, 0.0);
  cv::Mat sum_alpha2 = cv::Mat(rows, cols, CV_64FC1, 0.0);
  channels[0] = 0.0;
  channels[1] = 0.0;
  channels[2] = 0.0;
  cv::Mat tmp;
  for (size_t i = 3; i < mis.size(); i += 3) {
    sum_alpha0 += *(sis[i]);
    sum_alpha1 += *(sis[i + 1]);
    sum_alpha2 += *(sis[i + 2]);
    cv::multiply(*(sis[i]), *(mis[i]), tmp);
    channels[0] += tmp;
    cv::multiply(*(sis[i + 1]), *(mis[i + 1]), tmp);
    channels[1] += tmp;
    cv::multiply(*(sis[i + 2]), *(mis[i + 2]), tmp);
    channels[2] += tmp;
  }
  channels[0] /= sum_alpha0;
  channels[1] /= sum_alpha1;
  channels[2] /= sum_alpha2;

  for (size_t i = 0; i < mis.size(); i++) {
    delete mis.at(i);
    delete sis.at(i);
  }
}

// Compute anisotropy and eigen values/vector then applies filter to rgb image
void kuwaharaAnisotropicFilter(cv::Mat &rgb_image, std::vector<cv::Mat *> masks,
                               const cv::Mat &kernel) {
  cv::Mat channels[3];
  cv::split(rgb_image, channels);

  cv::Mat gray;
  cv::cvtColor(rgb_image, gray, cv::COLOR_BGR2GRAY);

  std::vector<cv::Mat *> structure_tensor =
      compute_structure_tensor(gray, kernel);
  std::vector<cv::Mat *> eigen_values = compute_eigen_values(
      structure_tensor[0], structure_tensor[1], structure_tensor[2]);

  // Compute the local orientation in the direction of the minor eigenvector
  cv::Mat eigen_vector1;
  cv::Mat eigen_vector2 = *(structure_tensor[1]) * -1.0;
  cv::Mat local_orientation = cv::Mat(gray.rows, gray.cols, CV_64FC1);

  cv::subtract(*(eigen_values[0]), *(structure_tensor[0]), eigen_vector1,
               cv::noArray(), CV_64FC1);
  for (int i = 0; i < gray.rows * gray.cols; i++) {
    local_orientation.at<double>(i) = static_cast<double>(
        std::atan2(eigen_vector1.at<double>(i), eigen_vector2.at<double>(i)));
  }
  local_orientation *= -1.0;

  // Compute the anisotropy of the image
  cv::Mat anisotropy;
  cv::Mat tmp;
  cv::subtract(*(eigen_values[0]), *(eigen_values[1]), anisotropy,
               cv::noArray(), CV_64FC1);
  cv::add(*(eigen_values[0]), *(eigen_values[1]), tmp, cv::noArray(), CV_64FC1);
  anisotropy /= tmp;

  _kuwaharaAnisotropicFilterGrey(channels, masks, anisotropy,
                                 local_orientation);
  cv::merge(channels, 3, rgb_image);

  delete structure_tensor[0];
  delete structure_tensor[1];
  delete structure_tensor[2];
  delete eigen_values[0];
  delete eigen_values[1];
}