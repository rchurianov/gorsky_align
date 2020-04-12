#pragma once

#include "matrix.h"

typedef Matrix<std::tuple<uint, uint, uint>> Image;

Image gaussian_separable(const Image& src_image, double sigma, int radius);
