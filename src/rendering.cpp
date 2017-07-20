#include "rendering.hpp"
#include "tgaimage.hpp"
#include "geometry.hpp"
#include "buffer.hpp"
#include "model.hpp"

void renderColourBuffer(const Buffer<TGAColor>& buffer, TGAImage& image) {
  for(int j=0; j<buffer.height; ++j) {
    for(int i=0; i<buffer.width; ++i) {
      image.set(i, j, buffer.get(i, j));
    }
  }
}

void renderZBuffer(const Buffer<float>& zBuffer, TGAImage& image) {
  for(int j=0; j<zBuffer.height; ++j) {
    for(int i=0; i<zBuffer.width; ++i) {
      float zVal = zBuffer.get(i, j);
      TGAColor grey = TGAColor(zVal, zVal, zVal, 255);
      image.set(i, j, grey);
    }
  }
}

Vec3f getBarycentricCoords(const Vec3f& A, const Vec3f& B, const Vec3f& C, const Vec3f& P) {
  Vec3f u = Vec3f(B.x - A.x, C.x - A.x, A.x - P.x).cross(Vec3f(B.y - A.y, C.y - A.y, A.y - P.y));
  if (std::abs(u.z)<1) return Vec3f(-1,1,1); // triangle is degenerate, in this case return smth with negative coordinates
  return Vec3f(1.f-(u.x+u.y)/u.z, u.x/u.z, u.y/u.z);
}

void renderWireFrame(const Model& model, Buffer<TGAColor>& buffer, const Matrix& MVP) {
  const TGAColor white = TGAColor(255, 255, 255, 255);
  for (int i=0; i<model.nfaces(); i++) {
    std::vector<int> face = model.face(i);
    for (int j=0; j<3; j++) {
      Vec3f v0 = m2v(MVP*v2m(model.vert(face[j])));
      Vec3f v1 = m2v(MVP*v2m(model.vert(face[(j+1)%3])));
      renderLine(v0.x, v0.y, v1.x, v1.y, buffer, white);
    }
  }
}
