#include "triangle.hpp"

bool Triangle::isVisible() {
  // In screen space
  return normal.z > 0;
}
