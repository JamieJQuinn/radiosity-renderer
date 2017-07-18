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
    Vertex operator+(Vertex v) {
      return Vertex(x + v.x, y + v.y, z + v.z);
    }
    Vertex operator-(Vertex v) {
      return Vertex(x - v.x, y - v.y, z - v.z);
    }
    Vertex operator*(real r) {
      return Vertex(x*r, y*r, z*r);
    }
    Vertex operator/(real r) {
      assert(r != 0.0f);
      return *this*(1.0f/r);
    }
    Vertex cross(Vertex v) {
      return Vertex(
        y*v.z - z*v.y,
        z*v.x - x*v.z,
        x*v.y - y*v.x
      );
    }
    real dot(Vertex v) {
      return x*v.x + y*v.y + z*v.z;
    }
    real length2() {
      return this->dot(*this);
    }
    real length() {
      return std::sqrt(length2());
    }
    Vertex normalise() {
      assert(length() > 0);
      return (*this)/length();
    }
    bool operator==(Vertex v) {
      return x == v.x and y == v.y and z == v.z;
    }
    bool operator!=(Vertex v) {
      return !(*this == v);
    }
    Vertex(const Vertex& v) {
      x = v.x;
      y = v.y;
      z = v.z;
    }
    friend std::ostream& operator<<(std::ostream&, const Vertex&);
};
