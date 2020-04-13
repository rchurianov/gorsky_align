
#include "sobel_x.h"
#include <vector>
#include <cmath>
#include <tuple>

using std::vector;
using std::tuple;
using std::tie;
using std::make_tuple;

class SobelX {
public:
    vector<int> kernel = {-1, 0, 1, -2, 0, 2, -1, 0, 1};
    static const int radius = 1;

    tuple<uint, uint, uint> operator () (const Image &m) const {
        uint size = 2 * radius + 1;
        uint r, g, b, sum_r = 0, sum_g = 0, sum_b = 0;

        for (uint i = 0; i < size; ++i) {
            for (uint j = 0; j < size; ++j) {
                tie(r, g, b) = m(i, j);
                r *= kernel[i * size + j];
                g *= kernel[i * size + j];
                b *= kernel[i * size + j];

                sum_r += r;
                sum_g += g;
                sum_b += b;
            }
        }

        return make_tuple(sum_r, sum_g, sum_b);
    }

};

Image sobel_x(const Image& src_image) {
    return src_image.unary_map(SobelX());
}
