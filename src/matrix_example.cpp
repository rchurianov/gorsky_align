#include <iostream>
#include "matrix.h"
#include "io.h"

using std::cout;
using std::endl;

using std::tuple;
using std::get;
using std::tie;
using std::make_tuple;

// Matrix usage example
// Also see: matrix.h, matrix.hpp for comments on how filtering works

class BoxFilterOp
{
public:
    tuple<uint, uint, uint> operator () (const Image &m) const
    {
        uint size = 2 * radius + 1;
        uint r, g, b, sum_r = 0, sum_g = 0, sum_b = 0;
        for (uint i = 0; i < size; ++i) {
            for (uint j = 0; j < size; ++j) {
                // Tie is useful for taking elements from tuple
                tie(r, g, b) = m(i, j);
                sum_r += r;
                sum_g += g;
                sum_b += b;
            }
        }
        auto norm = size * size;
        sum_r /= norm;
        sum_g /= norm;
        sum_b /= norm;
        return make_tuple(sum_r, sum_g, sum_b);
    }

    static const int radius = 1;
};

class GaussianFilterOp {
private:
    // pre-compute gaussian matrix
    // store it somewhere
    // for each i and j fetch matrix value
    // Radius of neighbourhoud, which is passed to that operator
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
    // recalculate matrix for sigma = 1.4
    uint gauss_1_4 [5][5]
    {
        {1, 2, 3, 2, 1},
        {2, 5, 6, 5, 2},
        {3, 6, 8, 6, 3},
        {2, 5, 6, 5, 2},
        {1, 2, 3, 2, 1}
    }; // for sigma = 1.4
public:
    tuple<uint, uint, uint> operator () (const Image &m) const {
        uint size = 2 * radius + 1;
        uint r, g, b, sum_r = 0, sum_g = 0, sum_b = 0;
        for (uint i = 0; i < size; ++i) {
            for (uint j = 0; j < size; ++j) {
                tie(r, g, b) = m(i, j);
                r *= gauss_1_4[i][j];
                g *= gauss_1_4[i][j];
                b *= gauss_1_4[i][j];

                sum_r += r;
                sum_g += g;
                sum_b += b;
            }
        }

        sum_r /= gauss_norm_1_4;
        sum_g /= gauss_norm_1_4;
        sum_b /= gauss_norm_1_4;

        return make_tuple(sum_r, sum_g, sum_b);
    }

    static const int radius = 2;
};

int main(int argc, char **argv)
{
    // Image = Matrix<tuple<uint, uint, uint>>
    // tuple is from c++ 11 standard
    Image img = load_image(argv[1]);
    Image img3;
    img3 = img;
    // Image img2 = img3.unary_map(BoxFilterOp());
    Image img2 = img3.unary_map(GaussianFilterOp());
    save_image(img2, argv[2]);
}
