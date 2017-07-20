#pragma once

#include "tgaimage.hpp"
#include "geometry.hpp"
#include "buffer.hpp"
#include "model.hpp"

void renderColourBuffer(const Buffer<TGAColor>& buffer, TGAImage& image);
void renderZBuffer(const Buffer<float>& zBuffer, TGAImage& image);
void renderWireFrame(const Model* model, Buffer<TGAColor>& buffer, int width, int height);
void renderWireFrame(const Model* model, Buffer<TGAColor>& buffer);
Vec3f getBarycentricCoords(const Vec3f& A, const Vec3f& B, const Vec3f& C, const Vec3f& P);

template <class T>
void renderLine(int x0, int y0, int x1, int y1, Buffer<T> &buffer, const T& fillValue) {
  bool steep = false;
  if (std::abs(x0-x1)<std::abs(y0-y1)) {
    std::swap(x0, y0);
    std::swap(x1, y1);
    steep = true;
  }
  if (x0>x1) {
    std::swap(x0, x1);
    std::swap(y0, y1);
  }

  for (int x=x0; x<=x1; x++) {
    float t = (x-x0)/(float)(x1-x0);
    int y = y0*(1.-t) + y1*t;
    if (steep) {
      buffer.set(y, x, fillValue);
    } else {
      buffer.set(x, y, fillValue);
    }
  }
}

template <class fillType, class zBufferType>
void renderTriangle(Vec3f *pts, Buffer<zBufferType>& zBuffer, Buffer<fillType> &buffer, const fillType& fillValue) {
  // Create bounding box
  Vec2f bboxmin(buffer.width-1, buffer.height-1);
  Vec2f bboxmax(0, 0);
  Vec2f clamp(buffer.width-1, buffer.height-1);
  for (int i=0; i<3; i++) {
    for (int j=0; j<2; j++) {
      // clip against buffer sides
      bboxmin[j] = std::max(0.f, std::min(bboxmin[j], pts[i][j]));
      bboxmax[j] = std::min(clamp[j], std::max(bboxmax[j], pts[i][j]));
    }
  }

  // Check every pixel in bounding box
  Vec3f P;
  for (P.x=bboxmin.x; P.x<=bboxmax.x; ++P.x) {
    for (P.y=bboxmin.y; P.y<=bboxmax.y; ++P.y) {
      Vec3f bc_screen = getBarycentricCoords(pts[0], pts[1], pts[2], P);
      if (bc_screen.x<0 || bc_screen.y<0 || bc_screen.z<0) continue;
      P.z = 0;
      for (int i=0; i<3; i++) P.z += pts[i].z*bc_screen[i];
      buffer.set(int(P.x), int(P.y), fillValue);
      if(zBuffer.get(int(P.x), int(P.y)) < P.z) {
        buffer.set(int(P.x), int(P.y), fillValue);
        zBuffer.set(int(P.x), int(P.y), P.z);
      }
    }
  }
}
