
#include "align.h"
#include "canny.h"
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

int get_upper_border(const Image& src_image) {
    // 5%
    int border = 0;
    int margin = src_image.n_rows * 15 / 100;
    pair<int, int> i_max_1 = {0, 0}; // row <-> max1
    pair<int, int> i_max_2 = {0, 0}; // rows <-> max2
    int count = 0;
    uint r, g, b;

    int start_i = 10;
    int end_j = src_image.n_cols;

    for (int i = start_i; i < margin; i++) {
        for (int j = 0; j < end_j; j++) {
            tie(r, g, b) = src_image(i, j);
            if (r > 0 && g > 0 && b > 0)
                ++count;
        }


        if (count > i_max_1.second) {
            i_max_2.first = i_max_1.first;
            i_max_2.second = i_max_1.second;

            i_max_1.first = i;
            i_max_1.second = count;
        }

        if (count < i_max_1.second && count > i_max_2.second) {
            i_max_2.first = i;
            i_max_2.second = count;
        }


        count = 0;
    }

    // cout << "i max 1 " << i_max_1.first << endl;
    // cout << "i max 2 " << i_max_2.first << endl;

    if (i_max_1.first > i_max_2.first) {
        border = i_max_1.first;
    } else {
        border = i_max_2.first;
    }

    return border; // image will start at border+1 from upper border
}

int get_left_border(const Image& src_image) {
    uint r, g, b;
    int border = 0;
    int margin = src_image.n_cols * 10 / 100;
    pair<int, int> j_max_1 = {0, 0};
    pair<int, int> j_max_2 = {0, 0};
    int count = 0;

    int start_j = 10;
    int end_i = src_image.n_rows;

    for (int j = start_j; j < margin; j++) {
        for (int i = 0; i < end_i; i++) {
            tie(r, g, b) = src_image(i, j);
            if (r > 0 && g > 0 && b > 0) {
                ++count;
            }
        }

        if (count > j_max_1.second) {
            j_max_2.first = j_max_1.first;
            j_max_2.second = j_max_1.second;

            j_max_1.first = j;
            j_max_1.second = count;
        }

        if (count < j_max_1.second && count > j_max_2.second) {
            j_max_2.first = j;
            j_max_2.second = count;
        }

        count = 0;
    }

    // cout << "j_max_1 " << j_max_1.first << endl;
    // cout << "j_max_2 " << j_max_2.first << endl;
    if (j_max_1.first > j_max_2.first) {
        border = j_max_1.first;
    } else {
        border = j_max_2.first;
    }

    return border;
}

int get_lower_border(const Image& src_image) {

    int border = 0;
    int count = 0;
    uint r, g, b;
    int margin = src_image.n_rows * 10 / 100;
    pair<int, int> i_max_1 = {0, 0};
    pair<int, int> i_max_2 = {0, 0};

    int start_i = src_image.n_rows - margin;
    int end_i = src_image.n_rows - 10;
    int end_j = src_image.n_cols;

    for (int i = start_i; i < end_i; i++) {
        for (int j = 0; j < end_j; j++) {
            tie(r, g, b) = src_image(i, j);
            if (r > 0 && g > 0 && b > 0) {
                ++count;
            }
        }

        if (count > i_max_1.second) {
            i_max_2.first = i_max_1.first;
            i_max_2.second = i_max_2.second;

            i_max_1.first = i;
            i_max_1.second = count;
        }

        if (count < i_max_1.second && count > i_max_2.second) {
            i_max_2.first = i;
            i_max_2.second = count;
        }

        count = 0;
    }

    if (i_max_1.first < i_max_2.first) {
        border = src_image.n_rows - i_max_1.first;
    } else {
        border = src_image.n_rows - i_max_2.first;
    }

    return border;
}

int get_right_border(const Image& src_image) {

    int count = 0;
    int border = 0; // return no of the border column starting from the rigth (1-based)
    int r, g, b;
    pair<int, int> j_max_1 = {0, 0};
    pair<int, int> j_max_2 = {0, 0};
    int margin = src_image.n_cols * 20 / 100;
    int end_margin = 10;

    int start_j = src_image.n_cols - margin - 1; // zero-based
    int end_j = src_image.n_cols - end_margin; // non-including
    int end_i = src_image.n_rows; // also for implicit conversion to int

    for (int j = start_j; j < end_j; j++) {
        for (int i = 0; i < end_i; i++) {
            tie(r, g, b) = src_image(i, j);
            if (r > 0 && g > 0 && b > 0) {
                ++count;
            }
        }

        if (count > j_max_1.second) {
            j_max_2.first = j_max_1.first;
            j_max_2.second = j_max_1.first;

            j_max_1.first = j;
            j_max_2.second = count;
        }

        if (count < j_max_1.second && count > j_max_2.second) {
            j_max_2.first = j;
            j_max_2.second = count;
        }

        count = 0;
    }

    if (j_max_1.first < j_max_2.first) {
        border = src_image.n_cols - j_max_1.first;
    } else {
        border = src_image.n_cols - j_max_2.first;
    }

    return border;
}

Image align(const Image& src_image) {
    // divide image into three images;
    int full_height = src_image.n_rows;
    int height = full_height / 3;
    int width = src_image.n_cols;

    Image blue(height, width);
    Image green(height, width);
    Image red(height, width);


    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            blue(i, j) = src_image(i, j);
        }
    }

    for (int i = height; i < 2 * height; i++) {
        for (int j = 0; j < width; j++) {
            green(i - height, j) = src_image(i, j);
        }
    }

    for (int i = 2 * height; i < 3 * height; i++) {
        for (int j = 0; j < width; j++) {
            red(i - 2 * height, j) = src_image(i, j);
        }
    }

    Image aligned(height, width);
    aligned = canny(blue, 50, 100);

    int upper_border = get_upper_border(blue); // zero base index
    int lower_border = get_lower_border(blue);
    int left_border = get_left_border(blue); // zero based index
    int right_border = get_right_border(blue);

    int new_height = height - (upper_border + 1) - (lower_border);
    int new_width = width - (left_border + 1) - (right_border);

    cout << "upper border " << upper_border << endl;
    cout << "left border " << left_border << endl;
    cout << "right border " << right_border << endl;
    cout << "lower border " << lower_border << endl;

    Image blue_wo_borders(new_height, new_width);

    /*
    0 1 2 3 4 5 6 7 8 9 10
            b
    left_border = b;
    new_width = 11 - (4 + 1);
    */

    for (int i = 0; i < new_height; i++) {
        for (int j = 0; j < new_width; j++) {
            blue_wo_borders(i, j) = blue(i + upper_border + 1, j + left_border + 1);
        }
    }

    return blue_wo_borders;
    // return blue;
}
