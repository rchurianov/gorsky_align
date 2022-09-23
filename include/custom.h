#pragma once

#include "matrix.h"

#include <tuple>

typedef Matrix<std::tuple<uint, uint, uint>> Image;

Image custom(const Image& src_image, const Matrix<double> kernel);
