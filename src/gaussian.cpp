
#include "gaussian.h"
#include <vector>
#include <cmath>
#include <tuple>

using std::vector;
using std::tuple;
using std::tie;
using std::make_tuple;

const double SIGMA = 1.4;
const double RADIUS = 2;
const double PI = atan(1) * 4;

class GaussianFilterOp {
private:
    static const uint gauss_norm_1_0 = 273;
    static const uint gauss_norm_1_4 = 84;

    uint gauss_1_0 [5][5]
    {
        {1, 4, 7, 4, 1},
        {4, 16, 26, 16, 4},
        {7, 26, 41, 26, 7},
        {4, 16, 26, 16, 4},
        {1, 4, 7, 4, 1}
    }; // for sigma = 1.0

    uint gauss_1_4 [5][5]
    {
        {1, 2, 3, 2, 1},
        {2, 5, 6, 5, 2},
        {3, 6, 8, 6, 3},
        {2, 5, 6, 5, 2},
        {1, 2, 3, 2, 1}
    }; // for sigma = 1.4

public:
    // static const int radius = 2;
    int radius = 0;
    int norm_factor = 0;
    // vector<unsigned int> kernel = {1, 2, 3, 2, 1, 2, 5, 6, 5, 2, 3, 6, 8, 6, 3, 2, 5, 6, 5, 2, 1, 2, 3, 2, 1};
    vector<unsigned int> kernel = {};

    // GaussianFilterOp() {
    //     radius = 2;
    //     norm_factor = 84;
    //     kernel = {};
    // }

    tuple<uint, uint, uint> operator () (const Image &m) const {
        uint size = 2 * radius + 1;
        uint r, g, b, sum_r = 0, sum_g = 0, sum_b = 0;

        for (uint i = 0; i < size; ++i) {
            for (uint j = 0; j < size; ++j) {
                tie(r, g, b) = m(i, j);
                r *= kernel[i* size + j];
                g *= kernel[i* size + j];
                b *= kernel[i* size + j];

                sum_r += r;
                sum_g += g;
                sum_b += b;
            }
        }

        sum_r /= norm_factor;
        sum_g /= norm_factor;
        sum_b /= norm_factor;

        return make_tuple(sum_r, sum_g, sum_b);
    }

    vector<double> gaussian_kernel_2d(const double sigma, const int r) {

        // double factor = 90;
        // double sigma_squared = 2.0;
        double factor = 1 / (2 * PI * sigma * sigma);
        // double factor = 1 / (2 * PI * sigma_squared);
        double divisor = 2.0 * sigma * sigma;
        // double divisor = 2.0 * sigma_squared;
        double sum = 0.0;

        // int number_of_elements = (2 * r + 1) * (2 * r + 1);
        vector<double> d_kernel;

        for (int i = 0; i < 2 * r + 1; i++) { // rows
            double y = static_cast<double>(i - r);
            for (int j = 0; j < 2 * r + 1; j++) { // columns
                double x = static_cast<double>(j - r);
                d_kernel.push_back(factor * exp(-((pow(x, 2.0) + pow(y, 2.0)) / divisor)));
                // d_kernel[i*(2*r + 1) + j] = exp(-0.5 * (pow(x/sigma, 2.0) + pow(y/sigma, 2.0))) / (2 * PI * sigma * sigma);
                sum += d_kernel[i*(2*r + 1) + j];
            }
        }

        /* Division by Sum Normalisation */
        /*
        for (double& d : d_kernel) {
            // d = d / sum;
            d *= 1000;
        }
        */
        // cout << "sum : " << sum << endl;


        /* Up Scale Normalization with the Smallest Element to 1 */
        double min = d_kernel[0];
        if (sum < 1) {
            double scale_up = 1.0 + (1.0 - min) / min;
            for (double& d : d_kernel ) {
                d *= scale_up;
            }
        }

        return d_kernel;
    }

    vector<unsigned int> get_integer_kernel_2d(const vector<double>& d_kernel) {
        vector<unsigned int> int_kernel;
        for (double d : d_kernel) {
            int_kernel.push_back(static_cast<unsigned int>(round(d)));
        }

        // int sum = 0;
        // for (int i : int_kernel) {
        //     sum += i;
        // }
        // cout << sum << endl;

        return int_kernel;
    }

    unsigned int get_norm_factor(const vector<unsigned int> int_kernel) {
        unsigned int sum = 0;
        for (unsigned int i : int_kernel) {
            sum += i;
        }

        return sum;
    }
};


Image gaussian(const Image& src_image, double sigma, int radius) {
    /*
    1. Calculate kernel
    2. Convolve src_image with kernel
    */
    Image dst_image;

    GaussianFilterOp op;
    op.radius = radius;

    vector<double> d_kernel;
    d_kernel = op.gaussian_kernel_2d(sigma, radius);
    op.kernel = op.get_integer_kernel_2d(d_kernel);
    op.norm_factor = op.get_norm_factor(op.kernel);

    dst_image = src_image.unary_map(op);
    // dst_image = src_image.unary_map(GaussianFilterOp());
    return dst_image;
}

/*
int main() {

    int number_of_elements = (2 * RADIUS + 1) * (2 * RADIUS + 1);
    vector<double> kernel_2d(number_of_elements);
    kernel_2d = gaussian_kernel_2d(SIGMA, RADIUS);

    cout << "2D Gaussian Kernel with Sigma = " << SIGMA << " Radius = " << RADIUS << endl;

    cout << setprecision(6) << fixed << setw(8);
    int shift = (2 * RADIUS + 1);
    for (int i = 0; i < 2 * RADIUS + 1; i++) {
        for (int j = 0; j < 2 * RADIUS + 1; j++) {
            cout << kernel_2d[i * shift + j] << " ";
        }
        cout << endl;
    }

    cout << endl;
    cout << "Integer Kernel:" << endl;
    vector<int> int_kernel_2d;
    int_kernel_2d = get_integer_kernel_2d(kernel_2d);
    for (int i = 0; i < 2 * RADIUS + 1; i++) {
        for (int j = 0; j < 2 * RADIUS + 1; j++) {
            cout << setw(2) << int_kernel_2d[i * shift + j] << " ";
        }
        cout << endl;
    }

    return 0;
}
*/
