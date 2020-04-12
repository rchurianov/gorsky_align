
#include "gaussian_separable.h"
#include <vector>
#include <cmath>
#include <tuple>

using std::vector;
using std::tuple;
using std::tie;
using std::make_tuple;

typedef unsigned int uint;

const double SIGMA = 1.4;
const double RADIUS = 2;
const double PI = atan(1) * 4;

class GaussianSeparableFilterOp {
public:
    int radius = 0;
    int norm_factor = 0; // sum of kernel elements
    vector<uint> kernel = {};

    tuple<uint, uint, uint> operator () (const Image &m) const {
        uint size = 2 * radius + 1;
        uint r, g, b, sum_r = 0, sum_g = 0, sum_b = 0;
        vector<uint> col_r(size);
        vector<uint> col_g(size);
        vector<uint> col_b(size);

        for (uint i = 0; i < size; ++i) {
            for (uint j = 0; j < size; ++j) {
                tie(r, g, b) = m(i, j);
                r *= kernel[j];
                g *= kernel[j];
                b *= kernel[j];

                col_r[i] += r;
                col_g[i] += g;
                col_b[i] += b;
            }
        }

        for (uint i = 0; i < size; ++i) {
            sum_r += col_r[i] * kernel[i];
            sum_g += col_g[i] * kernel[i];
            sum_b += col_b[i] * kernel[i];
        }
        sum_r /= (norm_factor * norm_factor);
        sum_g /= (norm_factor * norm_factor);
        sum_b /= (norm_factor * norm_factor);

        return make_tuple(sum_r, sum_g, sum_b);
    }

    vector<double> gaussian_kernel_1d(const double sigma, const int r) {
        vector<double> d_kernel;
        double factor = 1 / (sigma * sqrt(2 * PI));
        double divisor = 2 * sigma * sigma;
        double sum = 0.0;

        for (int i = -r; i <= r; i++) {
            double x = static_cast<double>(i);
            d_kernel.push_back(factor * exp(- x*x / divisor));
            sum += d_kernel[i + r];
        }

        /* Up Scale Normalization With Minimum Element to 1 */
        double min = d_kernel[0];
        if (sum < 1) {
            double scale_up = 1.0 + (1.0 - min) / min;
            for (double& d : d_kernel) {
                d *= scale_up;
            }
        }

        return d_kernel;
    }

    vector<unsigned int> get_integer_kernel_1d(const vector<double>& d_kernel) {
        vector<unsigned int> int_kernel;

        for (double d : d_kernel) {
            int_kernel.push_back(static_cast<unsigned int>(round(d)));
        }

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

Image gaussian_separable(const Image& src_image, double sigma, int radius) {
    Image dst_image;
    GaussianSeparableFilterOp op;
    op.radius = radius;

    vector<double> d_kernel;
    d_kernel = op.gaussian_kernel_1d(sigma, radius);
    op.kernel = op.get_integer_kernel_1d(d_kernel);
    op.norm_factor = op.get_norm_factor(op.kernel);

    dst_image = src_image.unary_map(op);

    return dst_image;
}
