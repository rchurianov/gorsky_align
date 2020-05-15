
#include "pyramid.h"

#include <iostream>

using std::cout;
using std::endl;

uint interpolated_value(uint f11, uint f12, uint f21, uint f22)
{
    uint tmp = f11 + f12 + f21 + f22;
    return static_cast<unsigned int>(tmp / 4);
}

Image scale_down(const Image& src_image) {

    cout << "original image " << src_image.n_rows << " x " << src_image.n_cols << endl;
    Image scaled_down(src_image.n_rows/2, src_image.n_cols/2);
    cout << "scaled_down h x w " << scaled_down.n_rows << " " << scaled_down.n_cols << endl;

    uint r1, g1, b1, r2, g2, b2, r3, g3, b3, r4, g4, b4;
    uint new_r, new_g, new_b;

    int end_i = src_image.n_rows;
    int end_j = src_image.n_cols;

    for (int i = 0; i < end_i-1; i=i+2) {
        for (int j = 0; j < end_j-1; j=j+2) {
            tie(r1, g1, b1) = src_image(i, j); // (y, x) = (0, 0)
            tie(r2, g2, b2) = src_image(i, j+1); // (y, x) = (1, 0)
            tie(r3, g3, b3) = src_image(i+1, j+1); // (y, x) = (1, 1)
            tie(r4, g4, b4) = src_image(i+1, j); // (y, x) = (0, 1)

            new_r = interpolated_value(r1, r4, r2, r3);
            new_g = interpolated_value(g1, g4, g2, g3);
            new_b = interpolated_value(b1, b4, b2, b3);

            scaled_down(i/2, j/2) = std::make_tuple(new_r, new_g, new_b);
        }
    }

    return scaled_down;
}
