
#include "align.h"
#include "canny.h"
#include "root_mean_square.h"
#include "cross_correlation.h"
#include "pyramid.h"

#include <vector>
#include <cmath>
#include <tuple>
#include <exception>
#include <utility> // pair
#include <iostream>
#include <limits>

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

Image align_two_images(const Image& image_1, const Image& image_2, int shift_i, int shift_j, bool red) {
    uint r1=0, g1=0, b1=0, r2=0, g2=0, b2=0;
    int start_i=0, end_i=0, start_j=0, end_j=0;
    int height=0, width=0;

    /*
    int height = image_1.n_rows < image_2.n_rows ? image_2.n_rows : image_1.n_rows;
    int width = image_1.n_cols < image_2.n_cols ? image_2.n_cols : image_1.n_cols;
    height = height + abs(shift_i);
    width = width + abs(shift_j);
    */

    if (shift_i >= 0 && shift_j <= 0) {
        height = image_2.n_rows <= abs(shift_i) + 1 + image_1.n_rows ? abs(shift_i) + 1 + image_1.n_rows : image_2.n_rows;
        width = image_2.n_cols <= abs(shift_j) + 1 + image_1.n_cols ? abs(shift_j) + 1 + image_1.n_cols : image_2.n_cols;
    } else if (shift_i >= 0 && shift_j > 0) {
        height = image_2.n_rows <= abs(shift_i) + 1 + image_1.n_rows ? abs(shift_i) + 1 + image_1.n_rows : image_2.n_rows;
        width = image_1.n_cols <= abs(shift_j) + 1 + image_2.n_cols ? abs(shift_j) + 1 + image_2.n_cols : image_1.n_cols;
    } else if (shift_i < 0 && shift_j <= 0) {
        height = image_1.n_rows <= abs(shift_i) + 1 + image_2.n_rows ? abs(shift_i) + 1 + image_2.n_rows : image_1.n_rows;
        width = image_2.n_cols <= abs(shift_j) + 1 + image_1.n_cols ? abs(shift_j) + 1 + image_1.n_cols : image_2.n_cols;
    } else if (shift_i < 0 && shift_j > 0) {
        height = image_1.n_rows <= abs(shift_i) + 1 + image_2.n_rows ? abs(shift_i) + 1 + image_2.n_rows : image_1.n_rows;
        width = image_1.n_cols <= abs(shift_j) + 1 + image_2.n_cols ? abs(shift_j) + 1 + image_2.n_cols : image_1.n_cols;
    }

    Image result(height, width);

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            result(i, j) = std::make_tuple(255, 255, 255);
        }
    }

    // copy first image

    if (shift_i >= 0 && shift_j <= 0) {
        start_i = abs(shift_i);
        end_i = start_i + image_1.n_rows;
        start_j = abs(shift_j);
        end_j = start_j + image_1.n_cols;
    } else if (shift_i >= 0 && shift_j > 0) {
        start_i = abs(shift_i);
        end_i = start_i + image_1.n_rows;
        start_j = 0;
        end_j = image_1.n_cols;
    } else if (shift_i < 0 && shift_j <= 0) {
        start_i = 0;
        end_i = image_1.n_rows;
        start_j = abs(shift_j);
        end_j = start_j + image_1.n_cols;
    } else if (shift_i < 0 && shift_j > 0) {
        start_i = 0;
        end_i = image_1.n_rows;
        start_j = 0;
        end_j = image_1.n_cols;
    }

    for (int i = start_i; i < end_i; i++) {
        for (int j = start_j; j < end_j; j++) {
            tie(r1, g1, b1) = image_1(i - start_i, j - start_j); // green or red
            result(i, j) = std::make_tuple(r1, g1, b1);
        }
    }


    // copy second image

    if (shift_i >= 0 && shift_j <= 0) {
        start_i = 0;
        end_i = image_2.n_rows;
        start_j = 0;
        end_j = image_2.n_cols;
    } else if (shift_i >= 0 && shift_j > 0) {
        start_i = 0;
        end_i = start_i + image_2.n_rows;
        start_j = abs(shift_j);
        end_j = start_j + image_2.n_cols;
    } else if (shift_i < 0 && shift_j <= 0) {
        start_i = abs(shift_i);
        end_i = start_i + image_2.n_rows;
        start_j = 0;
        end_j = shift_j + image_2.n_cols;
    } else if (shift_i < 0 && shift_j > 0) {
        start_i = abs(shift_i);
        end_i = start_i + image_2.n_rows;
        start_j = abs(shift_j);
        end_j = start_j + image_2.n_cols;
    }

    for (int i = start_i; i < end_i; i++) {
        for (int j = start_j; j < end_j; j++) {
            // cout << "( " << i << " " << j << " ) ";
            tie(r1, g1, b1) = image_2(i - start_i, j - start_j); // green or red
            result(i, j) = std::make_tuple(r1, g1, b1);
        }
    }


    // taking image1 as reference point
    start_i = abs(shift_i);
    start_j = abs(shift_j);

    if (shift_i >= 0 && shift_j <= 0) {
        end_i = image_2.n_rows <= start_i + image_1.n_rows ? image_2.n_rows : start_i + image_1.n_rows;
        end_j = image_2.n_cols <= start_j + image_1.n_cols ? image_2.n_cols : start_j + image_1.n_cols;
    } else if (shift_i >= 0 && shift_j > 0) {
        end_i = image_2.n_rows <= start_i + image_1.n_rows ? image_2.n_rows : start_i + image_1.n_rows;
        end_j = image_1.n_cols <= start_j + image_2.n_cols ? image_1.n_cols : start_j + image_1.n_cols;
    } else if (shift_i < 0 && shift_j <= 0) {
        end_i = image_1.n_rows <= start_i + image_2.n_rows ? image_1.n_rows : start_i + image_2.n_rows;
        end_j = image_2.n_cols <= start_j + image_1.n_cols ? image_2.n_cols : start_j + image_1.n_cols;
    } else if (shift_i < 0 && shift_j > 0) {
        end_i = image_1.n_rows <= start_i + image_2.n_rows ? image_1.n_rows : start_i + image_2.n_rows;
        end_j = image_1.n_cols <= start_j + image_2.n_cols ? image_1.n_cols : start_j + image_2.n_cols;
    }

    for (int i = start_i; i < end_i; i++) {
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
            // cout << "( " << r1 << " " << b1 << " " << g1 << " )" << " ; ";

            if (red) { // red is second image
                result(i, j) = make_tuple(r2, g1, b1);
            } else {
                result(i, j) = make_tuple(r1, g2, b1);
            }
        }
    }

    return result;
}

tuple<int, int, int, int> get_borders(const Image& src_image)
{
    int left_border = get_left_border(src_image); // zero based index
    int upper_border = get_upper_border(src_image); // zero base index
    int right_border = get_right_border(src_image);
    int lower_border = get_lower_border(src_image);

    return std::make_tuple(left_border, upper_border, right_border, lower_border);
}

tuple<int, int> get_new_dimensions(const Image& src_image, tuple<int, int, int, int>  new_borders) {
    int left_border=0, upper_border=0, right_border=0, lower_border=0;
    std::tie(left_border, upper_border, right_border, lower_border) = new_borders;

    int new_height = src_image.n_rows - (upper_border + 1) - (lower_border);
    int new_width = src_image.n_cols - (left_border + 1) - (right_border);

    return std::make_tuple(new_height, new_width);
}

Image crop(const Image& src_image, tuple<int, int, int, int> new_borders) {

    int left_border=0, upper_border=0, right_border=0, lower_border=0;
    std::tie(left_border, upper_border, right_border, lower_border) = get_borders(src_image);

    int new_height=0, new_width=0;
    std::tie(new_height, new_width) = get_new_dimensions(src_image, std::make_tuple(left_border,
                                                                                    upper_border,
                                                                                    right_border,
                                                                                    lower_border));

    Image result(new_height, new_width);

    for (int i = 0; i < new_height; i++) {
        for (int j = 0; j < new_width; j++) {
            result(i, j) = src_image(i + upper_border + 1, j + left_border + 1);
        }
    }

    return result;
}

tuple<int, int> get_minimal_shifts(const Image& first_image,
                                   const Image& second_image,
                                   int start_i,
                                   int end_i,
                                   int start_j,
                                   int end_j)
{
    double min_rms = std::numeric_limits<double>::max();
    double tmp_rms = 0.0;
    int min_i = 0;
    int min_j = 0;

    for (int shift_i = start_i; shift_i <= end_i; shift_i++) { // move to const
        for (int shift_j = start_j; shift_j <= end_j; shift_j++) {
            // cout << "( " << shift_i << " " << shift_j << ") " << endl;
            tmp_rms = root_mean_square(first_image, second_image, shift_i, shift_j);
            if (tmp_rms < min_rms) {
                min_rms = tmp_rms;
                min_i = shift_i;
                min_j = shift_j;
            }
        }
    }

    return std::make_tuple(min_i, min_j);
}


/*
 Main algorithm function - align
*/

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

    Image blue_edge_map(height, width);
    Image green_edge_map(height, width);
    Image red_edge_map(height, width);

    blue_edge_map = canny(blue, 50, 100);
    green_edge_map = canny(green, 50, 100);
    red_edge_map = canny(red, 50, 100);

    /*
    Remove film borders
    */

    // blue borders

    int new_height=0, new_width=0;
    tuple<int, int, int, int> borders = get_borders(blue);
    std::tie(new_height, new_width) = get_new_dimensions(blue, borders);

    Image blue_wo_borders(new_height, new_width);
    blue_wo_borders = crop(blue, borders);

    // green borders

    borders = get_borders(green);
    std::tie(new_height, new_width) = get_new_dimensions(green, borders);

    Image green_wo_borders(new_height, new_width);
    green_wo_borders = crop(green, borders);

    // red borders

    borders = get_borders(red);
    std::tie(new_height, new_width) = get_new_dimensions(red, borders);

    Image red_wo_borders(new_height, new_width);
    red_wo_borders = crop(red, borders);

    /*
    Align with image pyramid
    1. Scale down images by factor of two
    2. Get shifts for scaled down images
    3. Double the shifts and align original images
    */

    Image small_blue(blue_wo_borders.n_rows/2, blue_wo_borders.n_cols/2);
    Image small_green(green_wo_borders.n_rows/2, green_wo_borders.n_cols/2);

    small_blue = scale_down(blue_wo_borders);
    small_green = scale_down(green_wo_borders);

    // cout << "small_green: h x w " << small_green.n_rows << " " << small_green.n_cols << endl;
    // cout << "small_blue: h x w " << small_blue.n_rows << " " << small_green.n_cols << endl;

    int min_i=0, min_j=0;
    std::tie(min_i, min_j) = get_minimal_shifts(small_blue, small_green, -15, 15, -15, 15);

    // cout << "min i shift " << min_i << endl;
    // cout << "min j shift " << min_j << endl;

    int small_b_g_shift_i = min_i;
    int small_b_g_shift_j = min_j;

    if (min_i >= 0 && min_j <= 0) {
        new_height = small_green.n_rows <= abs(min_i) + 1 + small_blue.n_rows ? abs(min_i) + 1 + small_blue.n_rows : small_green.n_rows;
        new_width = small_green.n_cols <= abs(min_j) + 1 + small_blue.n_cols ? abs(min_j) + 1 + small_blue.n_cols : small_green.n_cols;
    } else if (min_i >= 0 && min_j > 0) {
        new_height = small_green.n_rows <= abs(min_i) + 1 + small_blue.n_rows ? abs(min_i) + 1 + small_blue.n_rows : small_green.n_rows;
        new_width = small_blue.n_cols <= abs(min_j) + 1 + small_green.n_cols ? abs(min_j) + 1 + small_green.n_cols : small_blue.n_cols;
    } else if (min_i < 0 && min_j <= 0) {
        new_height = small_blue.n_rows <= abs(min_i) + 1 + small_green.n_rows ? abs(min_i) + 1 + small_green.n_rows : small_blue.n_rows;
        new_width = small_green.n_cols <= abs(min_j) + 1 + small_blue.n_cols ? abs(min_j) + 1 + small_blue.n_cols : small_green.n_cols;
    } else if (min_i < 0 && min_j > 0) {
        new_height = small_blue.n_rows <= abs(min_i) + 1 + small_green.n_rows ? abs(min_i) + 1 + small_green.n_rows : small_blue.n_rows;
        new_width = small_blue.n_cols <= abs(min_j) + 1 + small_green.n_cols ? abs(min_j) + 1 + small_green.n_cols : small_blue.n_cols;
    }

    // cout << "new_height " << new_height << endl;
    // cout << "new_width " << new_width << endl;

    Image blue_green_match(new_height, new_width);

    blue_green_match = align_two_images(small_blue, small_green, min_i, min_j, false);

    // add red to blue_green_match

    Image small_red(red_wo_borders.n_rows/2, red_wo_borders.n_cols/2);

    small_red = scale_down(red_wo_borders);

    std::tie(min_i, min_j) = get_minimal_shifts(blue_green_match, small_red, -15, 15, -15, 15);

    int small_r_b_shift_i = min_i;
    int small_r_b_shift_j = min_j;

    // cout << "small red shifts" << endl;
    // cout << "min i shift " << min_i << endl;
    // cout << "min j shift " << min_j << endl;

    /*
    if (min_i >= 0 && min_j <= 0) {
        new_height = small_red.n_rows <= abs(min_i) + 1 + blue_green_match.n_rows ? abs(min_i) + 1 + blue_green_match.n_rows : small_red.n_rows;
        new_width = small_red.n_cols <= abs(min_j) + 1 + small_red.n_cols ? abs(min_j) + 1 + small_red.n_cols : small_red.n_cols;
    } else if (min_i >= 0 && min_j > 0) {
        new_height = small_red.n_rows <= abs(min_i) + 1 + blue_green_match.n_rows ? abs(min_i) + 1 + blue_green_match.n_rows : small_red.n_rows;
        new_width = small_red.n_cols <= abs(min_j) + 1 + blue_green_match.n_cols ? abs(min_j) + 1 + blue_green_match.n_cols : blue_green_match.n_cols;
    } else if (min_i < 0 && min_j <= 0) {
        new_height = blue_green_match.n_rows <= abs(min_i) + 1 + small_red.n_rows ? abs(min_i) + 1 + small_red.n_rows : blue_green_match.n_rows;
        new_width = small_red.n_cols <= abs(min_j) + 1 + blue_green_match.n_cols ? abs(min_j) + 1 + blue_green_match.n_cols : small_red.n_cols;
    } else if (min_i < 0 && min_j > 0) {
        new_height = blue_green_match.n_rows <= abs(min_i) + 1 + small_red.n_rows ? abs(min_i) + 1 + small_red.n_rows : blue_green_match.n_rows;
        new_width = blue_green_match.n_cols <= abs(min_j) + 1 + small_red.n_cols ? abs(min_j) + 1 + small_red.n_rows : blue_green_match.n_cols;
    }

    cout << "new_height " << new_height << endl;
    cout << "new_width " << new_width << endl;

    Image blue_red_match(new_height, new_width);

    blue_red_match = align_two_images(blue_green_match, small_red, min_i, min_j, true);
    */

    /*
    Align original images
    */

    int start_i = 2*small_b_g_shift_i - 7;
    int end_i = 2*small_b_g_shift_i + 7;
    int start_j = 2*small_b_g_shift_j - 7;
    int end_j = 2*small_b_g_shift_j + 7;

    std::tie(min_i, min_j) = get_minimal_shifts(blue_wo_borders, green_wo_borders, start_i, end_i, start_j, end_j);

    // cout << "blue green shift" << endl;
    // cout << "min i shift " << min_i << endl;
    // cout << "min j shift " << min_j << endl;

    if (min_i >= 0 && min_j <= 0) {
        new_height = green_wo_borders.n_rows <= abs(min_i) + 1 + blue_wo_borders.n_rows ? abs(min_i) + 1 + blue_wo_borders.n_rows : green_wo_borders.n_rows;
        new_width = green_wo_borders.n_cols <= abs(min_j) + 1 + blue_wo_borders.n_cols ? abs(min_j) + 1 + blue_wo_borders.n_cols : green_wo_borders.n_cols;
    } else if (min_i >= 0 && min_j > 0) {
        new_height = green_wo_borders.n_rows <= abs(min_i) + 1 + blue_wo_borders.n_rows ? abs(min_i) + 1 + blue_wo_borders.n_rows : green_wo_borders.n_rows;
        new_width = blue_wo_borders.n_cols <= abs(min_j) + 1 + green_wo_borders.n_cols ? abs(min_j) + 1 + green_wo_borders.n_cols : blue_wo_borders.n_cols;
    } else if (min_i < 0 && min_j <= 0) {
        new_height = blue_wo_borders.n_rows <= abs(min_i) + 1 + green_wo_borders.n_rows ? abs(min_i) + 1 + green_wo_borders.n_rows : blue_wo_borders.n_rows;
        new_width = green_wo_borders.n_cols <= abs(min_j) + 1 + blue_wo_borders.n_cols ? abs(min_j) + 1 + blue_wo_borders.n_cols : green_wo_borders.n_cols;
    } else if (min_i < 0 && min_j > 0) {
        new_height = blue_wo_borders.n_rows <= abs(min_i) + 1 + green_wo_borders.n_rows ? abs(min_i) + 1 + green_wo_borders.n_rows : blue_wo_borders.n_rows;
        new_width = blue_wo_borders.n_cols <= abs(min_j) + 1 + green_wo_borders.n_cols ? abs(min_j) + 1 + green_wo_borders.n_cols : blue_wo_borders.n_cols;
    }

    Image big_blue_green_match(new_height, new_width); // add dimensions

    big_blue_green_match = align_two_images(blue_wo_borders, green_wo_borders, min_i, min_j, false);

    // big blue - red match

    start_i = 2*small_r_b_shift_i - 7;
    end_i = 2*small_r_b_shift_i + 7;
    start_j = 2*small_r_b_shift_j - 7;
    end_j = 2*small_r_b_shift_j + 7;

    std::tie(min_i, min_j) = get_minimal_shifts(big_blue_green_match, red_wo_borders, start_i, end_i, start_j, end_j);

    // cout << "red shifts" << endl;
    // cout << "min i shift " << min_i << endl;
    // cout << "min j shift " << min_j << endl;

    // new_height = big_blue_green_match.n_rows > red_wo_borders.n_rows ? red_wo_borders.n_rows : big_blue_green_match.n_rows;
    // new_width = big_blue_green_match.n_cols > red_wo_borders.n_cols ? red_wo_borders.n_cols : big_blue_green_match.n_cols;
    //
    // new_height = new_height + abs(min_i);
    // new_width = new_width + abs(min_j);

    if (min_i >= 0 && min_j <= 0) {
        new_height = red_wo_borders.n_rows <= abs(min_i) + 1 + big_blue_green_match.n_rows ? abs(min_i) + 1 + big_blue_green_match.n_rows : red_wo_borders.n_rows;
        new_width = red_wo_borders.n_cols <= abs(min_j) + 1 + big_blue_green_match.n_cols ? abs(min_j) + 1 + big_blue_green_match.n_cols : red_wo_borders.n_cols;
    } else if (min_i >= 0 && min_j > 0) {
        new_height = red_wo_borders.n_rows <= abs(min_i) + 1 + big_blue_green_match.n_rows ? abs(min_i) + 1 + big_blue_green_match.n_rows : red_wo_borders.n_rows;
        new_width = big_blue_green_match.n_cols <= abs(min_j) + 1 + red_wo_borders.n_cols ? abs(min_j) + 1 + red_wo_borders.n_cols : big_blue_green_match.n_cols;
    } else if (min_i < 0 && min_j <= 0) {
        new_height = big_blue_green_match.n_rows <= abs(min_i) + 1 + red_wo_borders.n_rows ? abs(min_i) + 1 + red_wo_borders.n_rows : big_blue_green_match.n_rows;
        new_width = red_wo_borders.n_cols <= abs(min_j) + 1 + big_blue_green_match.n_cols ? abs(min_j) + 1 + big_blue_green_match.n_cols : red_wo_borders.n_cols;
    } else if (min_i < 0 && min_j > 0) {
        new_height = big_blue_green_match.n_rows <= abs(min_i) + 1 + red_wo_borders.n_rows ? abs(min_i) + 1 + red_wo_borders.n_rows : big_blue_green_match.n_rows;
        new_width = big_blue_green_match.n_cols <= abs(min_j) + 1 + red_wo_borders.n_cols ? abs(min_j) + 1 + red_wo_borders.n_cols : big_blue_green_match.n_cols;
    }

    Image big_blue_red_match(new_height, new_width);

    big_blue_red_match = align_two_images(big_blue_green_match, red_wo_borders, min_i, min_j, true);

    return big_blue_red_match;
    // return big_blue_green_match;

    // return blue_red_match;
    // return blue_green_match;

    // return small_green;

    // return blue_wo_borders;
    // return green_wo_borders;
    // return red_wo_borders;

    // return blue;
}
