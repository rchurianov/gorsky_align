
#include "cross_correlation.h"
#include <cmath>
#include <tuple>

using std::tuple;
using std::tie;

double cross_correlation(const Image& image_1, const Image& image_2) {
    uint r1, g1, b1, r2, g2, b2;
    double cc = 0.0, tmp_product = 0.0;

    int min_height = image_1.n_rows > image_2.n_rows ? image_2.n_rows : image_1.n_rows;
    int min_width = image_1.n_cols > image_2.n_cols ? image_2.n_cols : image_1.n_cols;

    for (int i = 0; i < min_height; i++) {
        for (int j = 0; j < min_width; j++) {
            tie(r1, g1, b1) = image_1(i, j);
            tie(r2, g2, b2) = image_2(i, j);
            tmp_product = static_cast<double>(r1*r2 + g1*g2 + b1*b2) / 256;
            cc += tmp_product;
        }
    }

    return cc;
}
