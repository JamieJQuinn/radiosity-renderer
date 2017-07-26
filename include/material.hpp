#pragma once

#include "geometry.hpp"

struct Material {
  Vec3f reflectivity;
  Vec3f emissivity;

  Material(const Vec3f& r, const Vec3f& e):
    reflectivity(r),
    emissivity(e)
  {}
  Material():
    reflectivity(Vec3f(0,0,0)),
    emissivity(Vec3f(0,0,0))
  {}
};
