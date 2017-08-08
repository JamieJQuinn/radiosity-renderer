#pragma once

#include <iostream>

#include "tgaimage.hpp"
#include "geometry.hpp"
#include "buffer.hpp"
#include "model.hpp"

void renderColourBuffer(const Buffer<TGAColor>& buffer, TGAImage& image);
void renderColourBuffer(const Buffer<TGAColor>& buffer, std::string filename);
void renderZBuffer(const Buffer<float>& zBuffer, TGAImage& image);
void renderZBuffer(const Buffer<float>& zBuffer, std::string filename);
void renderWireFrame(const Model& model, Buffer<TGAColor>& buffer, const Matrix& MVP);
void calcFormFactorPerCell(const int sideLengthInPixels, Buffer<float>& topFace, Buffer<float>& sideFace);
void calcFormFactorsFromBuffer(const Buffer<int>& itemBuffer, const Buffer<float>& factorsPerCell, std::vector<float>& formFactors);
void renderModel(Buffer<int>& buffer, const Model& model, const Matrix& MVP);
void renderModel(Buffer<TGAColor>& buffer, const Model& model, const Matrix& MVP);
void renderTestModelReflectivity(Buffer<TGAColor>& buffer, const Model& model, const Matrix& MVP);

Vec3f interpolate(const Vec3f& v0, const Vec3f& v1, float t);
float clipLineZ(const Vec3f& v0, const Vec3f& v1);
bool isVertexInFront(const Vec3f& v);
void transformFace(std::vector<Vec3f>& outScreenCoords, const Face& face, const Model& model, const Matrix& MVP);
TGAColor getFaceColour(const Face& face, const Model& model);
int clipTriangle(std::vector<Vec3f>& pts);
void calcBoundingBox(Vec2f& bboxmin, Vec2f& bboxmax, const Vec2f& clamp, const std::vector<Vec3f>& pts);

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

// No interpolation of fillValue
template <class fillType, class zBufferType>
void renderTriangle(const std::vector<Vec3f>& pts, Buffer<zBufferType>& zBuffer, Buffer<fillType> &buffer, const fillType& fillValue) {
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
      for (int i=0; i<3; i++) {
        P.z += pts[i].z*bc_screen[i];
      }
      if(zBuffer.get(int(P.x), int(P.y)) < P.z) {
        buffer.set(int(P.x), int(P.y), fillValue);
        zBuffer.set(int(P.x), int(P.y), P.z);
      }
    }
  }
}

// No ZBuffer
template <class fillType>
void renderTriangle(const std::vector<Vec3f>& pts, Buffer<fillType> &buffer, const fillType& fillValue) {
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
      buffer.set(int(P.x), int(P.y), fillValue);
    }
  }
}

// ZBuffer & Interpolation
template <class fillType, class zBufferType>
void renderTriangle(const std::vector<Vec3f>& pts, const float *intensities, Buffer<zBufferType>& zBuffer, Buffer<fillType> &buffer, const fillType& fillValue) {
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
      float intensity = 0.f;
      for (int i=0; i<3; i++) {
        P.z += pts[i].z*bc_screen[i];
        intensity += intensities[i]*bc_screen[i];
      }
      if(zBuffer.get(int(P.x), int(P.y)) < P.z) {
        buffer.set(int(P.x), int(P.y), fillValue*intensity);
        zBuffer.set(int(P.x), int(P.y), P.z);
      }
    }
  }
}

template <class fillType, class zBufferType>
void clipAndRenderTriangle(std::vector<Vec3f>& pts, Buffer<zBufferType>& zBuffer, Buffer<fillType> &buffer, const fillType& fillValue) {
  int numTriangles = clipTriangle(pts);
  for(int i=0; i<numTriangles; ++i) {
    // Transform into viewport
    Matrix viewport = viewportRelative(0, 0, buffer.width, buffer.height);
    for(int j=0; j<3; ++j) {
      pts[j] = applyTransform(viewport, pts[j]);
    }
    renderTriangle(pts, zBuffer, buffer, fillValue);
  }
}
