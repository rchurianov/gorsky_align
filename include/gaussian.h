#pragma once
// #ifndef GAUSS
// #define GAUSS
#include "matrix.h"

typedef Matrix<std::tuple<uint, uint, uint>> Image; // Image

Image gaussian(const Image& src_image, double sigma, int radius);

// #endif

// #include "gaussian.hpp"
