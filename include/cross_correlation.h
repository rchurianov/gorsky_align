#pragma once

#include "matrix.h"

typedef Matrix<tuple<uint, uint, uint>> Image;

double cross_correlation(const Image&, const Image&);
