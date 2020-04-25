
#include "canny.h"
#include "gaussian_separable.h"
#include "sobel_x.h"
#include "sobel_y.h"

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

typedef unsigned int uint;

const double PI = atan(1) * 4;

class Canny {};

Image modulus(const Image& x_grad, const Image& y_grad) {
    // std::cout << "x_grad rows: " << x_grad.n_rows << std::endl;
    // std::cout << "y_grad cols: " << y_grad.n_cols << std::endl;

    if (x_grad.n_rows != y_grad.n_rows ||
        x_grad.n_cols != y_grad.n_cols) {
        throw std::runtime_error("Gradient images' dimensions do not match");
    }
    uint r_x, g_x, b_x, r_y, g_y, b_y;
    uint mod_r, mod_g, mod_b;
    Image dst_image(x_grad.n_rows, x_grad.n_cols);

    const auto end_i = x_grad.n_rows;
    const auto end_j = x_grad.n_cols;

    for (uint i = 0; i < end_i; i++) {
        for (uint j = 0; j < end_j; j++) {
            tie(r_x, g_x, b_x) = x_grad(i, j);
            tie(r_y, g_y, b_y) = y_grad(i, j);
            mod_r = sqrt(r_x*r_x + r_y*r_y);
            mod_g = sqrt(g_x*g_x + g_y*g_y);
            mod_b = sqrt(b_x*b_x + b_y*b_y);
            dst_image(i, j) = make_tuple(mod_r, mod_g, mod_b);
        }
    }

    return dst_image;
}

Matrix<std::tuple<double, double, double>> grad_direction(const Image& x_grad, const Image& y_grad) {
    Matrix<std::tuple<double, double, double>> dst_image(x_grad.n_rows, x_grad.n_cols);

    uint r_x, g_x, b_x, r_y, g_y, b_y;
    double theta_r, theta_g, theta_b;
    auto end_i = x_grad.n_rows;
    auto end_j = x_grad.n_cols;

    for (uint i = 0; i < end_i; i++) {
        for (uint j = 0; j < end_j; j++) {
            tie(r_x, g_x, b_x) = x_grad(i, j);
            tie(r_y, g_y, b_y) = y_grad(i, j);
            theta_r = atan2(static_cast<double>(r_y), static_cast<double>(r_x));
            theta_g = atan2(static_cast<double>(g_y), static_cast<double>(g_x));
            theta_b = atan2(static_cast<double>(b_y), static_cast<double>(b_x));
            dst_image(i, j) = make_tuple(theta_r, theta_g, theta_b);
        }
    }

    return dst_image;
}

pair<int, int> get_shift_direction(double theta) {
    vector<pair<int, int>> plus_shift = {{1, 0}, {1, 1}, {0, 1}, {-1, 1}, {-1, 0}};
    vector<pair<int, int>> minus_shift = {{1, -1}, {0, -1}, {-1, -1}, {-1, 0}};

    double direction = 0.0;

    if (theta >= 0) {
        direction = (theta + PI/8) / (PI/4);
    } else {
        direction = (theta - PI/8) / (PI/4);
    }

    int int_dir = static_cast<int>(trunc(direction));

    if (int_dir >= 0) {
        return plus_shift[int_dir];
    } else {
        return minus_shift[(-1)*int_dir - 1];
    }
}

pair<int, int> get_opposite_direction(const pair<int, int> direction) {
    vector<pair<int, int>> plus_shift = {{1, 0}, {1, 1}, {0, 1}, {-1, 1}, {-1, 0}};
    vector<pair<int, int>> minus_shift = {{1, -1}, {0, -1}, {-1, -1}, {-1, 0}};

    pair<int, int> opposite;

    if (direction.first != 0) {
        opposite.first = - direction.first;
    }

    if (direction.second != 0) {
        opposite.second = - direction.second;
    }

    return opposite;
}

Image nonmaximum_suppresion(const Image& gradient, const Matrix<std::tuple<double, double, double>>& direction) {
    Image dst_image(gradient.n_rows, gradient.n_cols);

    uint grad_r, grad_g, grad_b;
    double dir_r, dir_g, dir_b;
    uint neigh_1_r, neigh_1_g, neigh_1_b;
    uint neigh_2_r, neigh_2_g, neigh_2_b;
    uint new_grad_r, new_grad_g, new_grad_b;

    const int radius = 1;
    const int start_i = radius;
    const int end_i = gradient.n_rows - radius;
    const int start_j = radius;
    const int end_j = gradient.n_cols - radius;

    // const double mod = PI / 4;

    for (int i = start_i; i < end_i; i++) {
        for (int j = start_j; j < end_j; j++) {
            // get current gradient module
            tie(grad_r, grad_g, grad_b) = gradient(i, j);

            // get element index in the direction of gradient
            tie(dir_r, dir_g, dir_b) = direction(i,  j);
            pair<int, int> red_shift = get_shift_direction(dir_r);
            pair<int, int> green_shift = get_shift_direction(dir_g);
            pair<int, int> blue_shift = get_shift_direction(dir_b);

            // get element index in the direction opposite of the gradient
            pair<int, int> red_op_shift = get_opposite_direction(red_shift);
            pair<int, int> green_op_shift = get_opposite_direction(green_shift);
            pair<int, int> blue_op_shift = get_opposite_direction(blue_shift);

            tie(neigh_1_r, neigh_1_g, neigh_1_b) = gradient(i+red_shift.second, j+red_shift.first);
            tie(neigh_2_r, neigh_2_g, neigh_2_b) = gradient(i+red_op_shift.second, j+red_op_shift.first);
            if (grad_r <= neigh_1_r) {
                new_grad_r = 0;
            } else if (grad_r <= neigh_2_r) {
                new_grad_r = 0;
            } else {
                new_grad_r = grad_r;
            }

            tie(neigh_1_r, neigh_1_g, neigh_1_b) = gradient(i+green_shift.second, j+green_shift.first);
            tie(neigh_2_r, neigh_2_g, neigh_2_b) = gradient(i+green_op_shift.second, j+green_op_shift.first);
            if (grad_g <= neigh_1_g) {
                new_grad_g = 0;
            } else if (grad_g <= neigh_2_g) {
                new_grad_g = 0;
            } else {
                new_grad_g = grad_g;
            }

            tie(neigh_1_r, neigh_1_g, neigh_1_b) = gradient(i+blue_shift.second, j+blue_shift.first);
            tie(neigh_2_r, neigh_2_g, neigh_2_b) = gradient(i+blue_op_shift.second, j+blue_op_shift.first);
            if (grad_b <= neigh_1_b) {
                new_grad_b = 0;
            } else if (grad_b <= neigh_2_b) {
                new_grad_b = 0;
            } else {
                new_grad_b = grad_b;
            }

            dst_image(i, j) = make_tuple(new_grad_r, new_grad_g, new_grad_b);
        }
    }

    return dst_image;
}

void follow_edge_red(const Image& gradient,
                     Image& edge_map,
                     uint i,
                     uint j,
                     uint threshold1,
                     uint threshold2,
                     vector<pair<uint, uint>>& shift)
{
    vector<pair<int, int>> queue;
    uint r, g, b;
    uint r_s, g_s, b_s; // values to store shift values
    uint r_edge, g_edge, b_edge;
    tie(r, g, b) = gradient(i, j);
    tie(r_edge, g_edge, b_edge) = edge_map(i, j); // save edge_map values

    // check if (i, j) red is included in the edge_map already
    if (r_edge > 0)
        return;

    // if (i, j) red is not included in the edge_map
    // add (i, j) red to the edge_map
    // add it to the queue
    edge_map(i, j) = make_tuple(r, g_edge, b_edge); // change only red values
    queue.push_back(make_pair(i, j));

    // while queue is not empty
    // pop element from the queue
    // check its neighbours
    // if a neighbour is bigger than threshold and is not in the edge_map
    // add it to the edge_map and to the queue

    while (!queue.empty()) {
        pair<int, int> pix = queue.back();
        queue.pop_back();

        for (uint k = 0; k < shift.size(); k++) {
            if (pix.first + shift[k].second < gradient.n_rows &&
                pix.second + shift[k].first < gradient.n_cols)
            {
                // check if the neighbour is not already labeled
                tie(r_edge, g_edge, b_edge) = edge_map(pix.first + shift[k].second, pix.second + shift[k].first);
                if (r_edge == 0) {
                    tie(r_s, g_s, b_s) = gradient(pix.first + shift[k].second, pix.second + shift[k].first);
                    if (r_s > threshold1) {
                        // label
                        edge_map(pix.first + shift[k].second, pix.second + shift[k].first) = make_tuple(r_s, g_edge, b_edge);
                        // add to queue
                        queue.push_back(make_pair(pix.first + shift[k].second, pix.second + shift[k].first));
                    }
                }
            }
        }
    }

    return;
}

void follow_edge_green(const Image& gradient,
                       Image& edge_map,
                       uint i,
                       uint j,
                       uint threshold1,
                       uint threshold2,
                       vector<pair<uint, uint>>& shift)
{
    vector<pair<int, int>> queue;
    uint r, g, b;
    uint r_s, g_s, b_s; // values to store shift values
    uint r_edge, g_edge, b_edge;
    tie(r, g, b) = gradient(i, j);
    tie(r_edge, g_edge, b_edge) = edge_map(i, j); // save edge_map values

    // check if (i, j) red is included in the edge_map already
    if (g_edge > 0)
        return;

    // if (i, j) red is not included in the edge_map
    // add (i, j) red to the edge_map
    // add it to the queue
    edge_map(i, j) = make_tuple(r_edge, g, b_edge); // change only green values
    queue.push_back(make_pair(i, j));

    // while queue is not empty
    // pop element from the queue
    // check its neighbours
    // if a neighbour is bigger than threshold and is not in the edge_map
    // add it to the edge_map and to the queue

    while (!queue.empty()) {
        pair<int, int> pix = queue.back();
        queue.pop_back();

        for (uint k = 0; k < shift.size(); k++) {
            if (pix.first + shift[k].second < gradient.n_rows &&
                pix.second + shift[k].first < gradient.n_cols)
            {
                // check if the neighbour is not already labeled
                tie(r_edge, g_edge, b_edge) = edge_map(pix.first + shift[k].second, pix.second + shift[k].first);
                if (g_edge == 0) {
                    tie(r_s, g_s, b_s) = gradient(pix.first + shift[k].second, pix.second + shift[k].first);
                    if (g_s > threshold1) {
                        // label
                        edge_map(pix.first + shift[k].second, pix.second + shift[k].first) = make_tuple(r_edge, g_s, b_edge);
                        // add to queue
                        queue.push_back(make_pair(pix.first + shift[k].second, pix.second + shift[k].first));
                    }
                }
            }
        }
    }

    return;
}

void follow_edge_blue(const Image& gradient,
                      Image& edge_map,
                      uint i,
                      uint j,
                      uint threshold1,
                      uint threshold2,
                      vector<pair<uint, uint>>& shift)
{
    vector<pair<int, int>> queue;
    uint r, g, b;
    uint r_s, g_s, b_s; // variables to store shift values
    uint r_edge, g_edge, b_edge;
    tie(r, g, b) = gradient(i, j);
    tie(r_edge, g_edge, b_edge) = edge_map(i, j); // save edge_map values

    // check if (i, j) red is included in the edge_map already
    if (b_edge > 0)
        return;

    // if (i, j) red is not included in the edge_map
    // add (i, j) red to the edge_map
    // add it to the queue
    edge_map(i, j) = make_tuple(r_edge, g_edge, b); // change only blue values
    queue.push_back(make_pair(i, j));

    // while queue is not empty
    // pop element from the queue
    // check its neighbours
    // if a neighbour is bigger than threshold and is not in the edge_map
    // add it to the edge_map and to the queue

    while (!queue.empty()) {
        pair<int, int> pix = queue.back();
        queue.pop_back();

        for (uint k = 0; k < shift.size(); k++) {
            if (pix.first + shift[k].second < gradient.n_rows &&
                pix.second + shift[k].first < gradient.n_cols)
            {
                // check if the neighbour is not already labeled
                tie(r_edge, g_edge, b_edge) = edge_map(pix.first + shift[k].second, pix.second + shift[k].first);
                if (b_edge == 0) {
                    tie(r_s, g_s, b_s) = gradient(pix.first + shift[k].second, pix.second + shift[k].first);
                    if (b_s > threshold1) {
                        // label
                        edge_map(pix.first + shift[k].second, pix.second + shift[k].first) = make_tuple(r_edge, g_edge, b_s);
                        // add to queue
                        queue.push_back(make_pair(pix.first + shift[k].second, pix.second + shift[k].first));
                    }
                }
            }
        }
    }

    return;
}


Image thresholding(const Image& gradient, uint threshold1, uint threshold2) {
    uint radius = 1;
    vector<pair<uint, uint>> shift = {{0,1}, {1, 1}, {0, 1}, {-1, 1}, {-1, 0}, {-1, -1}, {0, -1}, {1, -1}};

    cout << "thresholding: n x m " << gradient.n_rows << " " << gradient.n_cols << endl;

    Image edge_map(gradient.n_rows, gradient.n_cols);

    uint r, g, b;
    // uint new_r, new_g, new_b; // hope that Image constructor assigns zeros by default

    uint start_i = radius;
    uint start_j = radius;
    uint end_i = gradient.n_rows - radius;
    uint end_j = gradient.n_cols - radius;

    for (uint i = start_i; i < end_i; i++ ) {
        for (uint j = start_j; j < end_j; j++) {
            tie(r, g, b) = gradient(i, j);
            if (r < threshold1) {
                // new_r = 0;
            } else if (r > threshold2) {
                follow_edge_red(gradient, edge_map, i, j, threshold1, threshold2, shift);
            }

            if (g < threshold1) {
                // new_g = 0;
            } else if (g > threshold2) {
                follow_edge_green(gradient, edge_map, i, j, threshold1, threshold2, shift);
            }

            if (b < threshold1) {
                // new_b = 0;
            } else if (b > threshold2) {
                follow_edge_blue(gradient, edge_map, i, j, threshold1, threshold2, shift);
            }

            // edge_map(i, j) = make_tuple(new_r, new_g, new_b);
        }
    }

    return edge_map;
}


Image canny(const Image& src_image, uint threshold1, uint threshold2) {
    // std::cout << "canny" << std::endl;
    /*
    1. Smoothen the image
    2. Compute x and y derivatives
    3. Compute G = sqrt(x^2 + y^2)
    4. Compute gradient direction
    5. Nonmaximum suppression
    6. Double thresholding
    */
    Image smooth = gaussian_separable(src_image, 1.4, 2);
    Image x_derivative = sobel_x(smooth);
    Image y_derivative = sobel_y(smooth);
    Image gradient = modulus(x_derivative, y_derivative);
    Matrix<std::tuple<double, double, double>> direction = grad_direction(x_derivative, y_derivative);
    Image suppressed = nonmaximum_suppresion(gradient, direction);
    Image edge_map = thresholding(suppressed, threshold1, threshold2);
    return edge_map;
    // return suppressed;
    // return gradient;
}
