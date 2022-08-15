/*! \file anisotropic_kuwahara.cpp
    \brief Implementation of the anisotropic kuwahara filter
*/
#include "anisotropic_kuwahara.hh"

// Interpolation of coordinate x and y into the image
double bilinear_interpolation(const cv::Mat &image, double new_x,
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

void _bilinear_interpolation_anisotropic(cv::Mat *channels, cv::Mat *pow2,
                                         double weight, int i, int j,
                                         double new_x, double new_y,
                                         cv::Mat *mi0, cv::Mat *si0,
                                         cv::Mat *mi1, cv::Mat *si1,
                                         cv::Mat *mi2, cv::Mat *si2) {
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
  if (x2 >= channels[0].cols) {
    x2 -= 1.0;
  }
  if (y2 >= channels[0].rows) {
    y2 -= 1.0;
  }
  int index_x1 = static_cast<int>(x1);
  int index_x2 = static_cast<int>(x2);
  int index_y1 = static_cast<int>(y1);
  int index_y2 = static_cast<int>(y2);
  if (x1 == x2) {
    if (y1 == y2) {
      mi0->at<double>(i, j) +=
          weight * channels[0].at<double>(index_y1, index_x1);
      si0->at<double>(i, j) += weight * pow2[0].at<double>(index_y1, index_x1);
      mi1->at<double>(i, j) +=
          weight * channels[1].at<double>(index_y1, index_x1);
      si1->at<double>(i, j) += weight * pow2[1].at<double>(index_y1, index_x1);
      mi2->at<double>(i, j) +=
          weight * channels[2].at<double>(index_y1, index_x1);
      si2->at<double>(i, j) += weight * pow2[2].at<double>(index_y1, index_x1);
      return;
    }
    double cy1 = new_y - y1;
    double cy2 = y2 - new_y;

    mi0->at<double>(i, j) +=
        weight * (cy1 * channels[0].at<double>(index_y1, index_x1) +
                  cy2 * channels[0].at<double>(index_y2, index_x2));
    si0->at<double>(i, j) +=
        weight * (cy1 * pow2[0].at<double>(index_y1, index_x1) +
                  cy2 * pow2[0].at<double>(index_y2, index_x2));
    mi1->at<double>(i, j) +=
        weight * (cy1 * channels[1].at<double>(index_y1, index_x1) +
                  cy2 * channels[1].at<double>(index_y2, index_x2));
    si1->at<double>(i, j) +=
        weight * (cy1 * pow2[1].at<double>(index_y1, index_x1) +
                  cy2 * pow2[1].at<double>(index_y2, index_x2));
    mi2->at<double>(i, j) +=
        weight * (cy1 * channels[2].at<double>(index_y1, index_x1) +
                  cy2 * channels[2].at<double>(index_y2, index_x2));
    si2->at<double>(i, j) +=
        weight * (cy1 * pow2[2].at<double>(index_y1, index_x1) +
                  cy2 * pow2[2].at<double>(index_y2, index_x2));
    return;
  }
  double cx1 = new_x - x1;
  double cx2 = x2 - new_x;
  if (y1 == y2) {
    mi0->at<double>(i, j) +=
        weight * (cx1 * channels[0].at<double>(index_y1, index_x1) +
                  cx2 * channels[0].at<double>(index_y2, index_x2));
    si0->at<double>(i, j) +=
        weight * (cx1 * pow2[0].at<double>(index_y1, index_x1) +
                  cx2 * pow2[0].at<double>(index_y2, index_x2));
    mi1->at<double>(i, j) +=
        weight * (cx1 * channels[1].at<double>(index_y1, index_x1) +
                  cx2 * channels[1].at<double>(index_y2, index_x2));
    si1->at<double>(i, j) +=
        weight * (cx1 * pow2[1].at<double>(index_y1, index_x1) +
                  cx2 * pow2[1].at<double>(index_y2, index_x2));
    mi2->at<double>(i, j) +=
        weight * (cx1 * channels[2].at<double>(index_y1, index_x1) +
                  cx2 * channels[2].at<double>(index_y2, index_x2));
    si2->at<double>(i, j) +=
        weight * (cx1 * pow2[2].at<double>(index_y1, index_x1) +
                  cx2 * pow2[2].at<double>(index_y2, index_x2));
    return;
  }

  double cy2 = y2 - new_y;
  double cy1 = new_y - y1;

  double tmp_mi0 = cx2 * (cy2 * channels[0].at<double>(index_y2, index_x2) +
                          cy1 * channels[0].at<double>(index_y1, index_x2)) +
                   cx1 * (cy2 * channels[0].at<double>(index_y2, index_x1) +
                          cy1 * channels[0].at<double>(index_y1, index_x1));
  double tmp_si0 = cx2 * (cy2 * pow2[0].at<double>(index_y2, index_x2) +
                          cy1 * pow2[0].at<double>(index_y1, index_x2)) +
                   cx1 * (cy2 * pow2[0].at<double>(index_y2, index_x1) +
                          cy1 * pow2[0].at<double>(index_y1, index_x1));
  ;

  double tmp_mi1 = cx2 * (cy2 * channels[1].at<double>(index_y2, index_x2) +
                          cy1 * channels[1].at<double>(index_y1, index_x2)) +
                   cx1 * (cy2 * channels[1].at<double>(index_y2, index_x1) +
                          cy1 * channels[1].at<double>(index_y1, index_x1));
  ;
  double tmp_si1 = cx2 * (cy2 * pow2[1].at<double>(index_y2, index_x2) +
                          cy1 * pow2[1].at<double>(index_y1, index_x2)) +
                   cx1 * (cy2 * pow2[1].at<double>(index_y2, index_x1) +
                          cy1 * pow2[1].at<double>(index_y1, index_x1));
  ;

  double tmp_mi2 = cx2 * (cy2 * channels[2].at<double>(index_y2, index_x2) +
                          cy1 * channels[2].at<double>(index_y1, index_x2)) +
                   cx1 * (cy2 * channels[2].at<double>(index_y2, index_x1) +
                          cy1 * channels[2].at<double>(index_y1, index_x1));
  ;
  double tmp_si2 = cx2 * (cy2 * pow2[2].at<double>(index_y2, index_x2) +
                          cy1 * pow2[2].at<double>(index_y1, index_x2)) +
                   cx1 * (cy2 * pow2[2].at<double>(index_y2, index_x1) +
                          cy1 * pow2[2].at<double>(index_y1, index_x1));
  ;

  mi0->at<double>(i, j) += weight * tmp_mi0;
  si0->at<double>(i, j) += weight * tmp_si0;
  mi1->at<double>(i, j) += weight * tmp_mi1;
  si1->at<double>(i, j) += weight * tmp_si1;
  mi2->at<double>(i, j) += weight * tmp_mi2;
  si2->at<double>(i, j) += weight * tmp_si2;
}

// Applies an anisotropic kuwahara filter on greyscale image,
void _kuwaharaAnisotropicFilterGrey(cv::Mat *channels,
                                    std::vector<cv::Mat *> masks,
                                    const cv::Mat &anisotropy,
                                    const cv::Mat &local_orientation) {
  channels[0].convertTo(channels[0], CV_64FC1);
  channels[1].convertTo(channels[1], CV_64FC1);
  channels[2].convertTo(channels[2], CV_64FC1);

  std::vector<cv::Mat *> mis = std::vector<cv::Mat *>();
  std::vector<cv::Mat *> sis = std::vector<cv::Mat *>();

  cv::Mat channels_pow2[3];
  cv::pow(channels[0], 2, channels_pow2[0]);
  cv::pow(channels[1], 2, channels_pow2[1]);
  cv::pow(channels[2], 2, channels_pow2[2]);

  int rows = channels[0].rows;
  int cols = channels[0].cols;

  // Compute mi and si coefficients
  for (int x = 0; x < NB_SUBREGIONS; x++) {
    cv::Mat *mi0 = new cv::Mat(rows, cols, CV_64FC1, 0.0);
    cv::Mat *si0 = new cv::Mat(rows, cols, CV_64FC1, 0.0);
    cv::Mat *mi1 = new cv::Mat(rows, cols, CV_64FC1, 0.0);
    cv::Mat *si1 = new cv::Mat(rows, cols, CV_64FC1, 0.0);
    cv::Mat *mi2 = new cv::Mat(rows, cols, CV_64FC1, 0.0);
    cv::Mat *si2 = new cv::Mat(rows, cols, CV_64FC1, 0.0);
    cv::Mat *mask = masks.at(x);
    cv::parallel_for_(cv::Range(0, rows), [&](const cv::Range &range) {
      for (int i = range.start; i < range.end; i++) {
        for (int j = 0; j < cols; j++) {
          double cos_angle = cos(local_orientation.at<double>(i, j));
          double sin_angle = sin(local_orientation.at<double>(i, j));
          double iso = anisotropy.at<double>(i, j);
          for (int mx = 0; mx < 13; mx++) {
            double temp_i = static_cast<double>((mx - 6)) * iso;
            for (int mj = 0; mj < 13; mj++) {
              double weight = mask->at<double>(mx, mj);
              if (weight != 0.0) {
                double temp_j = static_cast<double>((mj - 6)) * (1.0 / iso);
                double new_x = (cos_angle * temp_j - sin_angle * temp_i) +
                               static_cast<double>(j);
                double new_y = (sin_angle * temp_j + cos_angle * temp_i) +
                               static_cast<double>(i);
                if (new_x >= 0.0 && new_x < cols && new_y >= 0.0 &&
                    new_y < rows) {
                  _bilinear_interpolation_anisotropic(
                      channels, channels_pow2, weight, i, j, new_x, new_y, mi0,
                      si0, mi1, si1, mi2, si2);
                }
              }
            }
          }
        }
      }
    });
    mis.emplace_back(mi0);
    mis.emplace_back(mi1);
    mis.emplace_back(mi2);
    sis.emplace_back(si0);
    sis.emplace_back(si1);
    sis.emplace_back(si2);
  }

  cv::parallel_for_(cv::Range(0, mis.size()), [&](const cv::Range &range) {
    for (int i = range.start; i < range.end; i++) {
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
  });

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

// Anisotropic Kuwahara Filter: Single-scale filtering
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

  cv::parallel_for_(
      cv::Range(0, gray.rows * gray.cols), [&](const cv::Range &range) {
        for (int i = range.start; i < range.end; i++) {
          local_orientation.at<double>(i) = static_cast<double>(std::atan2(
              eigen_vector1.at<double>(i), eigen_vector2.at<double>(i)));
        }
      });
  local_orientation *= -1.0;

  // Compute the anisotropy of the image
  cv::Mat anisotropy;
  cv::Mat tmp;
  cv::subtract(*(eigen_values[0]), *(eigen_values[1]), anisotropy,
               cv::noArray(), CV_64FC1);
  cv::add(*(eigen_values[0]), *(eigen_values[1]), tmp, cv::noArray(), CV_64FC1);
  anisotropy /= tmp;
  anisotropy += 1.0;

  _kuwaharaAnisotropicFilterGrey(channels, masks, anisotropy,
                                 local_orientation);
  cv::merge(channels, 3, rgb_image);

  delete structure_tensor[0];
  delete structure_tensor[1];
  delete structure_tensor[2];
  delete eigen_values[0];
  delete eigen_values[1];
}