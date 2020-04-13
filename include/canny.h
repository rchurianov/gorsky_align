#pragma once

#include "matrix.h"

typedef Matrix<std::tuple<uint, uint, uint>> Image;

Image canny(const Image& src_image, double threshold1, double threshold2);
