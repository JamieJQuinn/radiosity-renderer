#pragma once

#include "precision.hpp"
#include "buffer.hpp"
#include "geometry.hpp"
#include "triangle.hpp"

class Renderer {
  public:
    Renderer(int _width, int _height);
    Renderer(int _width, int _height, real initialZ);

    void fillTriangle(Triangle& t);

    real getZ(int i, int j) const;
    Triangle* getTriangle(int i, int j) const;

    friend std::ostream& operator<<(std::ostream&, const Renderer&);

  private:
    Buffer<real> zBuffer;
    Buffer<Triangle*> itemBuffer;
    int width, height;

    void fillLine(int x1, int x2, int y, real z, Triangle& tri);
    void fillBottomFlatTriangle(const Vec3f& v1, const Vec3f& v2, const Vec3f& v3, Triangle& tri);
    void fillTopFlatTriangle(const Vec3f& v1, const Vec3f& v2, const Vec3f& v3, Triangle& tri);
};