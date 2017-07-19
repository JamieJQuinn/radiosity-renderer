#pragma once

#include "geometry.hpp"

// Uses counter-clockwise normal system.
// This is a right-handed rule, so if you curl your hand around the
// triangle to follow v1 then v2 then v3, your thumb points in direction
// of normal.

class Triangle {
  public:
    Vec3f v1, v2, v3, normal;
    Triangle(Vec3f& _v1, Vec3f& _v2, Vec3f& _v3):
      v1(_v1),
      v2(_v2),
      v3(_v3),
      normal((v2 - v1).cross(v3 - v1).normalise())
    {}
    bool isVisible();
};
