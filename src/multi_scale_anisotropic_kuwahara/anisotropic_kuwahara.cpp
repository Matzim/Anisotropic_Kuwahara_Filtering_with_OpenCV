/*! \file anisotropic_kuwahara.cpp
    \brief Implementation of the anisotropic kuwahara filter
*/
#include "anisotropic_kuwahara.hh"

// Interpolation of coordinate x and y into the image
void _bilinear_interpolation_anisotropic(cv::Mat *channels, cv::Mat *pow2,
                                         double weight, int i, int j,
                                         double new_x, double new_y,
                                         cv::Mat *mis, cv::Mat *sis, int index, int index1, int index2) {
  double x1 = floor(new_x);
  double x2 = ceil(new_x);
  double y1 = floor(new_y);
  double y2 = ceil(new_y);
  int index_x1 = static_cast<int>(x1);
  int index_x2 = static_cast<int>(x2);
  int index_y1 = static_cast<int>(y1);
  int index_y2 = static_cast<int>(y2);
  double cx1 = new_x - x1;
  double cx2 = x2 - new_x;
  double cy1 = new_y - y1;
  double cy2 = y2 - new_y;
  
  double tmp_mi0 = cx2 * (cy2 * channels[0].at<double>(index_y2, index_x2) +
                          cy1 * channels[0].at<double>(index_y1, index_x2)) +
                   cx1 * (cy2 * channels[0].at<double>(index_y2, index_x1) +
                          cy1 * channels[0].at<double>(index_y1, index_x1));
  double tmp_si0 = cx2 * (cy2 * pow2[0].at<double>(index_y2, index_x2) +
                          cy1 * pow2[0].at<double>(index_y1, index_x2)) +
                   cx1 * (cy2 * pow2[0].at<double>(index_y2, index_x1) +
                          cy1 * pow2[0].at<double>(index_y1, index_x1));

  double tmp_mi1 = cx2 * (cy2 * channels[1].at<double>(index_y2, index_x2) +
                          cy1 * channels[1].at<double>(index_y1, index_x2)) +
                   cx1 * (cy2 * channels[1].at<double>(index_y2, index_x1) +
                          cy1 * channels[1].at<double>(index_y1, index_x1));
  double tmp_si1 = cx2 * (cy2 * pow2[1].at<double>(index_y2, index_x2) +
                          cy1 * pow2[1].at<double>(index_y1, index_x2)) +
                   cx1 * (cy2 * pow2[1].at<double>(index_y2, index_x1) +
                          cy1 * pow2[1].at<double>(index_y1, index_x1));

  double tmp_mi2 = cx2 * (cy2 * channels[2].at<double>(index_y2, index_x2) +
                          cy1 * channels[2].at<double>(index_y1, index_x2)) +
                   cx1 * (cy2 * channels[2].at<double>(index_y2, index_x1) +
                          cy1 * channels[2].at<double>(index_y1, index_x1));
  double tmp_si2 = cx2 * (cy2 * pow2[2].at<double>(index_y2, index_x2) +
                          cy1 * pow2[2].at<double>(index_y1, index_x2)) +
                   cx1 * (cy2 * pow2[2].at<double>(index_y2, index_x1) +
                          cy1 * pow2[2].at<double>(index_y1, index_x1));

  mis[index].at<double>(i, j) += weight * tmp_mi0;
  sis[index].at<double>(i, j) += weight * tmp_si0;
  mis[index1].at<double>(i, j) += weight * tmp_mi1;
  sis[index1].at<double>(i, j) += weight * tmp_si1;
  mis[index2].at<double>(i, j) += weight * tmp_mi2;
  sis[index2].at<double>(i, j) += weight * tmp_si2;
}

// Applies an anisotropic kuwahara filter on greyscale image,
void _kuwaharaAnisotropicFilterGrey(cv::Mat *channels,
                                    std::vector<cv::Mat *> masks,
                                    const cv::Mat &anisotropy,
                                    cv::Mat &local_orientation) {
  channels[0].convertTo(channels[0], CV_64FC1);
  channels[1].convertTo(channels[1], CV_64FC1);
  channels[2].convertTo(channels[2], CV_64FC1);

  cv::Mat channels_pow2[3];
  cv::pow(channels[0], 2, channels_pow2[0]);
  cv::pow(channels[1], 2, channels_pow2[1]);
  cv::pow(channels[2], 2, channels_pow2[2]);

  int rows = channels[0].rows;
  int cols = channels[0].cols;

  cv::Mat mis[NB_SUBREGIONS * 3];
  cv::Mat sis[NB_SUBREGIONS * 3];

  cv::Mat local_orientation_sin = cv::Mat(rows, cols, CV_64FC1, 0.0);
  cv::parallel_for_(cv::Range(0, rows * cols), [&](const cv::Range &range) {
    for (int i = range.start; i < range.end; i++) {
      local_orientation_sin.at<double>(i) = sin(local_orientation.at<double>(i));
      local_orientation.at<double>(i) = cos(local_orientation.at<double>(i));
    }
  });

  // Compute mi and si coefficients
  cv::parallel_for_(cv::Range(0, NB_SUBREGIONS), [&](const cv::Range &range) {
    for (int x = range.start; x < range.end; x++) {
      int index = x * 3;
      int index1 = index + 1;
      int index2 = index + 2;
      mis[index] = cv::Mat(rows, cols, CV_64FC1, 0.0);
      sis[index] = cv::Mat(rows, cols, CV_64FC1, 0.0);
      mis[index1] = cv::Mat(rows, cols, CV_64FC1, 0.0);
      sis[index1] = cv::Mat(rows, cols, CV_64FC1, 0.0);
      mis[index2] = cv::Mat(rows, cols, CV_64FC1, 0.0);
      sis[index2] = cv::Mat(rows, cols, CV_64FC1, 0.0);

      cv::Mat *mask = masks.at(x);

      for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
          double cos_angle = local_orientation.at<double>(i, j);
          double sin_angle = local_orientation_sin.at<double>(i, j);
          double iso = anisotropy.at<double>(i, j);
          for (int mx = 0; mx < 13; mx++) {
            double temp_i = static_cast<double>((mx - 6)) * iso;
            for (int mj = 0; mj < 13; mj++) {
              double weight = mask->at<double>(mx, mj);
              if (weight != 0.0) {
                double temp_j = static_cast<double>((mj - 6)) * (1.0 / iso);
                double new_x = (cos_angle * temp_j - sin_angle * temp_i) + static_cast<double>(j);
                if (new_x >= 0.0 && new_x <= (cols - 1)) {
                  double new_y = (sin_angle * temp_j + cos_angle * temp_i) + static_cast<double>(i);
                  if (new_y >= 0.0 && new_y <= (rows - 1)) {
                    _bilinear_interpolation_anisotropic(
                      channels, channels_pow2, weight, i, j, new_x, new_y, mis, sis, index, index1, index2);
                  }
                }
              }
            }
          }
        }
      }
      cv::Mat tmp0;
      cv::Mat tmp1;
      cv::Mat tmp2;
      cv::pow(mis[index], 2, tmp0);
      cv::pow(mis[index1], 2, tmp1);
      cv::pow(mis[index2], 2, tmp2);
      cv::absdiff(sis[index], tmp0, sis[index]);
      cv::absdiff(sis[index1], tmp1, sis[index1]);
      cv::absdiff(sis[index2], tmp2, sis[index2]);
      cv::sqrt(sis[index], sis[index]);
      cv::sqrt(sis[index1], sis[index1]);
      cv::sqrt(sis[index2], sis[index2]);
      cv::pow(sis[index], SHARPENESS_PARAMETER, sis[index]);
      cv::pow(sis[index1], SHARPENESS_PARAMETER, sis[index1]);
      cv::pow(sis[index2], SHARPENESS_PARAMETER, sis[index2]);
      sis[index] = (1.0 / (sis[index] + 1));
      sis[index1] = (1.0 / (sis[index1] + 1));
      sis[index2] = (1.0 / (sis[index2] + 1));
    }
  });

  cv::Mat sum_alpha0 = cv::Mat(rows, cols, CV_64FC1, 0.0);
  cv::Mat sum_alpha1 = cv::Mat(rows, cols, CV_64FC1, 0.0);
  cv::Mat sum_alpha2 = cv::Mat(rows, cols, CV_64FC1, 0.0);
  channels[0] = 0.0;
  channels[1] = 0.0;
  channels[2] = 0.0;
  cv::Mat tmp;
  for (size_t i = 3; i < 3 * NB_SUBREGIONS; i += 3) {
    sum_alpha0 += (sis[i]);
    sum_alpha1 += (sis[i + 1]);
    sum_alpha2 += (sis[i + 2]);
    cv::multiply(sis[i], mis[i], tmp);
    channels[0] += tmp;
    cv::multiply(sis[i + 1], mis[i + 1], tmp);
    channels[1] += tmp;
    cv::multiply(sis[i + 2], mis[i + 2], tmp);
    channels[2] += tmp;
  }
  channels[0] /= sum_alpha0;
  channels[1] /= sum_alpha1;
  channels[2] /= sum_alpha2;
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
              eigen_vector1.at<double>(i), eigen_vector2.at<double>(i))) * -1.0;
        }
      });

  // Compute the anisotropy of the image
  cv::Mat anisotropy;
  cv::Mat tmp;
  cv::subtract(*(eigen_values[0]), *(eigen_values[1]), anisotropy,
               cv::noArray(), CV_64FC1);
  cv::add(*(eigen_values[0]), *(eigen_values[1]), tmp, cv::noArray(), CV_64FC1);
  anisotropy /= tmp;
  anisotropy += 1.0;

  _kuwaharaAnisotropicFilterGrey(channels, masks, anisotropy, local_orientation);
  cv::merge(channels, 3, rgb_image);

  delete structure_tensor[0];
  delete structure_tensor[1];
  delete structure_tensor[2];
  delete eigen_values[0];
  delete eigen_values[1];
}