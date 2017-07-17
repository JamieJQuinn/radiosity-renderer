#pragma once
#include "precision.hpp"

class Vertex {
  public:
    real x, y, z;
    Vertex(real _x, real _y, real _z) {
      x = _x;
      y = _y;
      z = _z;
    }
    Vertex(real _x, real _y) {
      x = _x;
      y = _y;
      z = 0.0f;
    }
};
