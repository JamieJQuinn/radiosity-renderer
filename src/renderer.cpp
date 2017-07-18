#include <iostream>
#include "renderer.hpp"
#include "precision.hpp"
#include "buffer.hpp"
#include "vertex.hpp"

Renderer::Renderer(int _width, int _height, real initialZ):
  zBuffer(_width, _height, initialZ),
  itemBuffer(_width, _height, NULL),
  width(_width),
  height(_height)
{}

Renderer::Renderer(int _width, int _height):
  zBuffer(_width, _height, 0.0f),
  itemBuffer(_width, _height, NULL),
  width(_width),
  height(_height)
{}

real Renderer::getZ(int i, int j) const {
  return zBuffer.get(i, j);
}

Triangle* Renderer::getTriangle(int i, int j) const {
  return itemBuffer.get(i, j);
}

void Renderer::fillLine(int x1, int x2, int y, real z, Triangle& tri) {
  for(int x=x1; x<=x2; ++x) {
    if(z >= zBuffer.get(x, y)) {
      zBuffer.set(x, y, z);
      itemBuffer.set(x, y, &tri);
    }
    z -= tri.normal.x/tri.normal.z;
  }
}

void Renderer::fillTriangle(Triangle& tri) {
  // Assume vertices partially sorted in y dir (i.e. tri.v1.y <= tri.v2.y, tri.v3.y)
  assert(tri.v1.y <= tri.v2.y);
  assert(tri.v1.y <= tri.v3.y);

  // Handle redundant triangle
  if(tri.v1 == tri.v2 or tri.v2 == tri.v3 or tri.v1 == tri.v3) {
    return;
  }

  Vertex &upper = tri.v1;
  Vertex &mid = tri.v2.y <= tri.v3.y ? tri.v2 : tri.v3;
  Vertex &lower = tri.v2.y <= tri.v3.y ? tri.v3 : tri.v2;

  // check for trivial case of bottom-flat triangle
  if (mid.y == lower.y)
  {
    if(mid.x > lower.x) {
      fillBottomFlatTriangle(upper, mid, lower, tri);
    } else {
      fillBottomFlatTriangle(upper, lower, mid, tri);
    }
  }
  // check for trivial case of top-flat triangle 
  else if (mid.y == upper.y)
  {
    // Check order of vertices
    if(upper.x < mid.x) {
      fillTopFlatTriangle(upper, mid, lower, tri);
    } else {
      fillTopFlatTriangle(mid, upper, lower, tri);
    }
  }
  else
  {
    // general case - split the triangle in a topflat and bottom-flat one
    Vertex v4((int)(upper.x + ((real)(mid.y - upper.y) / (real)(lower.y - upper.y)) * (lower.x - upper.x)), mid.y);
    if(v4.x < mid.x) {
      fillBottomFlatTriangle(upper, mid, v4, tri);
      fillTopFlatTriangle(v4, mid, lower, tri);
    } else {
      fillBottomFlatTriangle(upper, v4, mid, tri);
      fillTopFlatTriangle(mid, v4, lower, tri);
    }
  }
}

void Renderer::fillBottomFlatTriangle(const Vertex& v1, const Vertex& v2, const Vertex& v3, Triangle& tri) {
  assert(v2.y == v3.y);
  assert(v1.y < v2.y);

  real invslope1 = (v2.x - v1.x) / (v2.y - v1.y);
  real invslope2 = (v3.x - v1.x) / (v3.y - v1.y);

  real curx1 = v1.x;
  real curx2 = v1.x;

  real D = Vertex(tri.v1.x, tri.v1.y, tri.v1.z).dot(tri.normal);
  real z = (D - tri.normal.x*curx2 - tri.normal.y*v1.y)/tri.normal.z;
  for (int scanlineY = v1.y; scanlineY <= v2.y; scanlineY++)
  {
    fillLine((int)curx2, (int)curx1, scanlineY, z, tri);
    curx1 += invslope1;
    curx2 += invslope2;
    z -= (tri.normal.x*invslope2 + tri.normal.y)/tri.normal.z;
  }
}

void Renderer::fillTopFlatTriangle(const Vertex& v1, const Vertex& v2, const Vertex& v3, Triangle& tri) {
  assert(v1.y == v2.y);
  assert(v1.y < v3.y);

  real invslope1 = (v3.x - v1.x) / (v3.y - v1.y);
  real invslope2 = (v3.x - v2.x) / (v3.y - v2.y);

  real curx1 = v3.x;
  real curx2 = v3.x;

  real D = Vertex(tri.v1.x, tri.v1.y, tri.v1.z).dot(tri.normal);
  real z = (D - tri.normal.x*curx2 - tri.normal.y*v3.y)/tri.normal.z;
  for (int scanlineY = v3.y; scanlineY > v1.y; scanlineY--)
  {
    z = (D - tri.normal.x*curx2 - tri.normal.y*v3.y)/tri.normal.z;
    fillLine((int)curx1, (int)curx2, scanlineY, z, tri);
    curx1 -= invslope1;
    curx2 -= invslope2;
    z += (tri.normal.x*invslope1 + tri.normal.y)/tri.normal.z;
  }
}

std::ostream& operator<<(std::ostream & stream, const Renderer& r){
  for(int j=0; j<r.height; ++j) {
    for(int i=0; i<r.width; ++i) {
      stream << r.zBuffer.get(i, j);
    }
    stream << std::endl;
  }
  return stream;
}
