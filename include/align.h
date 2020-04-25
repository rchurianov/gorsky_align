#pragma once

#include "matrix.h"

typedef Matrix<std::tuple<uint, uint, uint>> Image;

Image align(const Image& src_image);
