#include "utils.hh"

// Apply gauss function to all values in matrix
void apply_gauss(cv::Mat* mask, double coeff) {
    int middle = mask->rows / 2;

    for (int i = 0; i < mask->rows; i++) {
        double* Mi = mask->ptr<double>(i);
        for (int j = 0; j < mask->cols; j++) {
            if (Mi[j] > 0) {
                Mi[j] *= coeff * (
                    exp(((pow(i - middle, 2) + pow(j - middle, 2)) / (2.0 * pow(STANDARD_DEVIATION, 2))) * -1.0)
                );
            }
        }
    }
}

// Extract sub-matrix of size (13, 13) from matrix mat.
// Extraction is centered on matrix mat own center
cv::Mat* extractMatrix(cv::Mat *mat) {
    cv::Mat* res = new cv::Mat(13, 13, CV_64FC1);
    for (int i = 0; i < 13; i++) {
        for (int j = 0; j < 13; j++) {
            res->at<double>(i,j) = mat->at<double>(i + 5, 5 + j);
        }
    }
    delete mat;
    return res;
}