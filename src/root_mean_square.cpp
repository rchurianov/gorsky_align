
#include "root_mean_square.h"
#include <vector>
#include <cmath>
#include <tuple>
#include <exception>
#include <utility> // pair
#include <iostream>

using std::vector;
using std::tuple;
using std::tie;
using std::make_tuple;
using std::make_pair;
using std::pair;
using std::cout;
using std::endl;

double root_mean_square(const Image& image_1, const Image& image_2) {
    uint r1, g1, b1, r2, g2, b2;
    double rms = 0.0, tmp_sum = 0.0;
    int min_height = image_1.n_rows > image_2.n_rows ? image_2.n_rows : image_1.n_rows;
    int min_width = image_1.n_cols > image_2.n_rows ? image_2.n_cols : image_1.n_cols;

    for (int i = 0; i < min_height; i++) {
        for (int j = 0; j < min_width; j++) {
            tie(r1, g1, b1) = image_1(i, j);
            tie(r2, g2, b2) = image_2(i, j);
            tmp_sum = (r1 - r2) * (r1 - r2) + (g1 - g2) * (g1 - g2) + (b1 - b2) * (b1 - b2);
            rms += static_cast<double>(tmp_sum) / static_cast<double>(min_height * min_width);
        }
    }

    return rms;
}
