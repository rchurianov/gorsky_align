#pragma once

#include "matrix.h"

typedef Matrix<std::tuple<uint, uint, uint>> Image;

Image sobel_x(const Image& src_image);
