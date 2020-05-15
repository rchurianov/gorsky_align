#pragma once

#include "matrix.h"

#include <tuple>

using std::tie;
using std::make_tuple;

typedef Matrix<tuple<uint, uint, uint>> Image;

Image scale_down(const Image&);
