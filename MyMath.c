//
// Created by kir on 09.05.2025.
//
#include "MyMath.h"

#include <math.h>

double arccot(const double x) {
  if (x == 0.0) {
    return M_PI_2;
  }
  const double atan_val = atan(1.0 / x);
  return (x > 0) ? atan_val : (M_PI + atan_val);
}
