
#include "canny.h"
#include "gaussian_separable.h"
#include "sobel_x.h"
#include "sobel_y.h"

#include <vector>
#include <cmath>
#include <tuple>
#include <exception>
// #include <iostream>

using std::vector;
using std::tuple;
using std::tie;
using std::make_tuple;
// using std::cout;
// using std::endl;

class Canny {};

Image modulus(const Image& x_gradient, const Image& y_gradient) {
    // std::cout << "x_gradient rows: " << x_gradient.n_rows << std::endl;
    // std::cout << "y_gradient cols: " << y_gradient.n_cols << std::endl;

    if (x_gradient.n_rows != y_gradient.n_rows ||
        x_gradient.n_cols != y_gradient.n_cols) {
        throw std::runtime_error("Gradient images' dimensions do not match");
    }
    uint r_x, g_x, b_x, r_y, g_y, b_y;
    uint mod_r, mod_g, mod_b;
    Image dst_image(x_gradient.n_rows, x_gradient.n_cols);

    const auto end_i = x_gradient.n_rows;
    const auto end_j = x_gradient.n_cols;

    for (uint i = 0; i < end_i; i++) {
        for (uint j = 0; j < end_j; j++) {
            tie(r_x, g_x, b_x) = x_gradient(i, j);
            tie(r_y, g_y, b_y) = y_gradient(i, j);
            mod_r = sqrt(r_x*r_x + r_y*r_y);
            mod_g = sqrt(g_x*g_x + g_y*g_y);
            mod_b = sqrt(b_x*b_x + b_y*b_y);
            dst_image(i, j) = make_tuple(mod_r, mod_g, mod_b);
        }
    }

    return dst_image;
}

Image grad_direction(const Image& x_gradient, const Image& y_gradient) {
    
}

Image canny(const Image& src_image, double threshold1, double threshold2) {
    // std::cout << "canny" << std::endl;
    /*
    1. Smoothen the image
    2. Compute x and y derivatives
    3. Compute G = sqrt(x^2 + y^2)
    4. Compute gradient direction
    */
    Image smooth = gaussian_separable(src_image, 1.4, 2);
    Image x_derivative = sobel_x(smooth);
    Image y_derivative = sobel_y(smooth);
    Image mod = modulus(x_derivative, y_derivative);
    return modulus(x_derivative, y_derivative);
}
