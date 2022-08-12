/*! \file local_structure_estimation.cpp
    \brief Implementation of the local structure estimation
*/
#include "local_structure_estimation.hh"

std::vector<cv::Mat*> gauss_derivatives(cv::Mat& img, cv::Mat& Dx, cv::Mat& Dy) {
    cv::Mat *partial_derivative_x = new cv::Mat();
    cv::Mat *partial_derivative_y = new cv::Mat();

    cv::filter2D(img, *partial_derivative_x, CV_32F, Dx);
    cv::filter2D(img, *partial_derivative_y, CV_32F, Dy);
    return std::vector<cv::Mat*>({partial_derivative_x, partial_derivative_y});
}

std::vector<cv::Mat*> compute_structure_tensor(cv::Mat channels[3]) {
    cv::Mat Dx = (cv::Mat_<double>(3,3) << 0.0915, 0, -0.0915, 0.317, 0, -0.317, 0.0915, 0, -0.0915);
    cv::Mat Dy;
    cv::transpose(Dx, Dy);

    // Calculate approximations of the partial derivative
    std::vector<cv::Mat*> r_deriv = gauss_derivatives(channels[0], Dx, Dy);
    std::vector<cv::Mat*> g_deriv = gauss_derivatives(channels[1], Dx, Dy);
    std::vector<cv::Mat*> b_deriv = gauss_derivatives(channels[2], Dx, Dy);

    cv::Mat *E = new cv::Mat();
    cv::Mat *F = new cv::Mat();
    cv::Mat *G = new cv::Mat();

    cv::Mat dest;

    cv::pow(*r_deriv[0], 2, dest);
    *E = dest;
    *F = (r_deriv[0])->mul(*r_deriv[1]);
    cv::pow(*r_deriv[1], 2, dest);
    *G = dest;

    cv::pow(*g_deriv[0], 2, dest);
    *E += dest;
    *F += (g_deriv[0])->mul(*g_deriv[1]);
    cv::pow(*g_deriv[1], 2, dest);
    *G += dest;

    cv::pow(*b_deriv[0], 2, dest);
    *E += dest;
    *F += (b_deriv[0])->mul(*b_deriv[1]);
    cv::pow(*b_deriv[1], 2, dest);
    *G += dest;

    delete r_deriv[0];
    delete r_deriv[1];
    delete g_deriv[0];
    delete g_deriv[1];
    delete b_deriv[0];
    delete b_deriv[1];
    return std::vector<cv::Mat*>({E, F, G});
}

std::vector<cv::Mat*> compute_eigen_values(cv::Mat* E, cv::Mat* F, cv::Mat* G) {
    cv::Mat EG = *E + *G;
    cv::Mat sub;
    cv::Mat sqrt;

    cv::subtract(*E, *G, sub, cv::noArray(), CV_32F);
    cv::pow(sub, 2, sub);
    cv::pow(*F, 2, sqrt);
    sqrt *= 4.0;
    cv::sqrt(sqrt + sub, sqrt);

    cv::Mat *l1 = new cv::Mat();
    cv::Mat *l2 = new cv::Mat();

    *l1 = EG + sqrt;
    *l2 = EG - sqrt;
    *l1 /= 2.0;
    *l2 /= 2.0;
    return std::vector<cv::Mat*>({l1, l2});
}