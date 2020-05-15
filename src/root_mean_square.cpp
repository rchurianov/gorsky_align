
#include "root_mean_square.h"
#include <cmath>
#include <tuple>
#include <iostream>

using std::tuple;
using std::tie;
using std::cout;
using std::endl;

double root_mean_square(const Image& image_1, const Image& image_2, int shift_i, int shift_j) {
    uint r1=0, g1=0, b1=0, r2=0, g2=0, b2=0;
    double rms = 0.0, tmp_sum = 0.0;
    int height=0, width=0, start_i=0, start_j=0, end_i=0, end_j=0;

    // int min_height = image_1.n_rows > image_2.n_rows ? image_2.n_rows : image_1.n_rows;
    // int min_width = image_1.n_cols > image_2.n_cols ? image_2.n_cols : image_1.n_cols;
    //
    // min_height = min_height - abs(shift_i);
    // min_width = min_width - abs(shift_j);

    // if (shift_i >= 0 && shift_j <= 0) {
    //     height = image_2.n_rows <= abs(shift_i) + 1 + image_1.n_rows ? abs(shift_i) + 1 + image_1.n_rows : image_2.n_rows;
    //     width = image_2.n_cols <= abs(shift_j) + 1 + image_1.n_cols ? abs(shift_j) + 1 + image_1.n_cols : image_2.n_cols;
    // } else if (shift_i >= 0 && shift_j > 0) {
    //     height = image_2.n_rows <= abs(shift_i) + 1 + image_1.n_rows ? abs(shift_i) + 1 + image_1.n_rows : image_2.n_rows;
    //     width = image_1.n_cols <= abs(shift_j) + 1 + image_2.n_cols ? abs(shift_j) + 1 + image_2.n_cols : image_1.n_cols;
    // } else if (shift_i < 0 && shift_j <= 0) {
    //     height = image_1.n_rows <= abs(shift_i) + 1 + image_2.n_rows ? abs(shift_i) + 1 + image_2.n_rows : image_1.n_rows;
    //     width = image_2.n_cols <= abs(shift_j) + 1 + image_1.n_cols ? abs(shift_j) + 1 + image_1.n_cols : image_2.n_cols;
    // } else if (shift_i < 0 && shift_j > 0) {
    //     height = image_1.n_rows <= abs(shift_i) + 1 + image_2.n_rows ? abs(shift_i) + 1 + image_2.n_rows : image_1.n_rows;
    //     width = image_1.n_cols <= abs(shift_j) + 1 + image_2.n_cols ? abs(shift_j) + 1 + image_2.n_cols : image_1.n_cols;
    // }

    // taking image1 as reference point
    start_i = abs(shift_i);
    start_j = abs(shift_j);

    if (shift_i >= 0 && shift_j <= 0) {
        end_i = image_2.n_rows <= start_i + image_1.n_rows ? image_2.n_rows : start_i + image_1.n_rows;
        end_j = image_2.n_cols <= start_j + image_1.n_cols ? image_2.n_cols : start_j + image_1.n_cols;
    } else if (shift_i >= 0 && shift_j > 0) {
        end_i = image_2.n_rows <= start_i + image_1.n_rows ? image_2.n_rows : start_i + image_1.n_rows;
        end_j = image_1.n_cols <= start_j + image_2.n_cols ? image_1.n_cols : start_j + image_2.n_cols;
    } else if (shift_i < 0 && shift_j <= 0) {
        end_i = image_1.n_rows <= start_i + image_2.n_rows ? image_1.n_rows : start_i + image_2.n_rows;
        end_j = image_2.n_cols <= start_j + image_1.n_cols ? image_2.n_cols : start_j + image_1.n_cols;
    } else if (shift_i < 0 && shift_j > 0) {
        end_i = image_1.n_rows <= start_i + image_2.n_rows ? image_1.n_rows : start_i + image_2.n_rows;
        end_j = image_1.n_cols <= start_j + image_2.n_cols ? image_1.n_cols : start_j + image_2.n_cols;
    }

    // cout << "min_height " << min_height << endl;
    // cout << "min_width " << min_width << endl;

    height = end_i - start_i;
    width = end_j - start_j;

    for (int i = start_i; i < end_i; i++) { // 0..min_height-1
        for (int j = start_j; j < end_j; j++) {
            // cout << "( " << i << " " << j << " ) ";
            if (shift_i >= 0 && shift_j <= 0) {
                tie(r1, g1, b1) = image_1(i - start_i, j - start_j);
                tie(r2, g2, b2) = image_2(i, j);
            } else if (shift_i >= 0 && shift_j > 0) {
                tie(r1, g1, b1) = image_1(i - start_i, j);
                tie(r2, g2, b2) = image_2(i, j - start_j);
            } else if (shift_i < 0 && shift_j <= 0) {
                tie(r1, g1, b1) = image_1(i, j - start_j);
                tie(r2, g2, b2) = image_2(i - start_i, j);
            } else if (shift_i < 0 && shift_j > 0) {
                tie(r1, g1, b1) = image_1(i, j);
                tie(r2, g2, b2) = image_2(i - start_i, j - start_j);
            }

            tmp_sum = (r1 - r2) * (r1 - r2) + (g1 - g2) * (g1 - g2) + (b1 - b2) * (b1 - b2);
            rms += static_cast<double>(tmp_sum) / static_cast<double>(height * width);
        }
    }

    return rms;
}
