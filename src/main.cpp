#include <vector>
#include <cmath>
#include <iostream>
#include "buffer.hpp"
#include "tgaimage.hpp"
#include "model.hpp"
#include "geometry.hpp"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0,   0,   255);
const TGAColor black   = TGAColor(0, 0,   0,   255);
Model *model = NULL;
const int width  = 800;
const int height = 800;

template <class T>
void line(int x0, int y0, int x1, int y1, Buffer<T> &buffer, T fillValue) {
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

template void line<TGAColor>(int, int, int, int, Buffer<TGAColor>&, TGAColor);

template <class T>
void renderFromBuffer(const Buffer<T>& buffer, TGAImage& image) {
  for(int j=0; j<buffer.height; ++j) {
    for(int i=0; i<buffer.width; ++i) {
      image.set(i, j, buffer.get(i, j));
    }
  }
}

template void renderFromBuffer<TGAColor>(const Buffer<TGAColor>& buffer, TGAImage& image);

Vec3f getBarycentricCoords(const Vec3f& A, const Vec3f& B, const Vec3f& C, const Vec3f& P) {
  Vec3f u = Vec3f(B.x - A.x, C.x - A.x, A.x - P.x).cross(Vec3f(B.y - A.y, C.y - A.y, A.y - P.y));
  if (std::abs(u.z)<1) return Vec3f(-1,1,1); // triangle is degenerate, in this case return smth with negative coordinates
  return Vec3f(1.f-(u.x+u.y)/u.z, u.y/u.z, u.x/u.z);
}

template <class T>
void triangle(Vec3f *pts, Buffer<float>& zBuffer, Buffer<T> &buffer, T fillValue) {
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

template void triangle(Vec3f *pts, Buffer<float>&, Buffer<TGAColor>&, TGAColor);

void renderZBuffer(const Buffer<float>& zBuffer, TGAImage& image) {
  for(int j=0; j<zBuffer.height; ++j) {
    for(int i=0; i<zBuffer.width; ++i) {
      float zVal = zBuffer.get(i, j);
      TGAColor grey = TGAColor(zVal, zVal, zVal, 255);
      image.set(i, j, grey);
    }
  }
}

int main(int argc, char** argv) {
  Buffer<TGAColor> buffer(200, 200, white);
  Buffer<float> zBuffer(200, 200, 0);
  Vec3f pts[3] = {Vec3f(10,10, 244), Vec3f(100, 30, 0), Vec3f(190, 160, 100)};
  triangle(pts, zBuffer, buffer, red);

  TGAImage frame(200, 200, TGAImage::RGB);
  //renderFromBuffer(buffer, frame);
  renderZBuffer(zBuffer, frame);
  frame.flip_vertically(); // to place the origin in the bottom left corner of the image
  frame.write_tga_file("output.tga");
  return 0;
}

int drawWireFrame(int argc, char** argv) {
  if (2==argc) {
    model = new Model(argv[1]);
  } else {
    std::cerr << "No input obj" << std::endl;
  }

  Buffer<TGAColor> buffer(width, height, black);
  for (int i=0; i<model->nfaces(); i++) {
    std::vector<int> face = model->face(i);
    for (int j=0; j<3; j++) {
      Vec3f v0 = model->vert(face[j]);
      Vec3f v1 = model->vert(face[(j+1)%3]);
      int x0 = (v0.x+1.)*width/2.;
      int y0 = (v0.y+1.)*height/2.;
      int x1 = (v1.x+1.)*width/2.;
      int y1 = (v1.y+1.)*height/2.;
      line(x0, y0, x1, y1, buffer, white);
    }
  }

  TGAImage image(width, height, TGAImage::RGB);
  renderFromBuffer(buffer, image);
  image.flip_vertically();
  image.write_tga_file("output.tga");

  delete model;
  return 0;
}

