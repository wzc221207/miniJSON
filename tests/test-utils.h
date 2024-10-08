// Copyright (c) 2024 Zhichen (Joshua) Wen
#pragma once
#include <cmath>
#include <limits>

inline bool double_equal(double a, double b) {
  return std::fabs(a - b) < std::numeric_limits<double>::epsilon();
}
