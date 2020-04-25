
#include "align.h"
#include "canny.h"

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

Image align(const Image& src_image) {
    // divide image into three images;
    int height = src_image.n_rows / 3;
    int width = src_image.n_cols;
    Image blue(height, width);
    Image green(height, width);
    Image red(height, width);

    Image aligned(height, width);

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            blue(i, j) = src_image(i, j);
        }
    }

    for (int i = height; i < 2 * height; i++) {
        for (int j = 0; j < width; j++) {
            green(i, j) = src_image(i, j);
        }
    }

    for (int i = 2 * height; i < 3 * height; i++) {
        for (int j = 0; j < width; j++) {
            red(i, j) = src_image(i, j);
        }
    }

    aligned = blue;

    return aligned;
}
