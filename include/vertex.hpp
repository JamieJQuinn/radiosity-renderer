#pragma once

#include <cassert>
#include <cmath>
#include <iostream>
#include "precision.hpp"

class Vertex {
  public:
    real x, y, z, _dummy;
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
    Vertex operator+(const Vertex& v) const {
      return Vertex(x + v.x, y + v.y, z + v.z);
    }
    Vertex operator-(const Vertex& v) const {
      return Vertex(x - v.x, y - v.y, z - v.z);
    }
    Vertex operator*(const real& r) const {
      return Vertex(x*r, y*r, z*r);
    }
    Vertex operator/(const real& r) const {
      assert(r != 0.0f);
      return *this*(1.0f/r);
    }
    Vertex cross(const Vertex& v) const {
      return Vertex(
        y*v.z - z*v.y,
        z*v.x - x*v.z,
        x*v.y - y*v.x
      );
    }
    real dot(const Vertex& v) const {
      return x*v.x + y*v.y + z*v.z;
    }
    real length2() const {
      return this->dot(*this);
    }
    real length() const {
      return std::sqrt(length2());
    }
    Vertex normalise() const {
      assert(length() > 0);
      return (*this)/length();
    }
    bool operator==(const Vertex& v) const {
      return x == v.x and y == v.y and z == v.z;
    }
    bool operator!=(const Vertex& v) const {
      return !(*this == v);
    }
    Vertex(const Vertex& v) {
      x = v.x;
      y = v.y;
      z = v.z;
    }
    friend std::ostream& operator<<(std::ostream&, const Vertex&);
};
