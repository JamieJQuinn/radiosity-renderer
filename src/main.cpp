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
void horizontalLine(int x0, int x1, int y, Buffer<T> &buffer, T fillValue) {
  if (x0>x1) {
    std::swap(x0, x1);
  }
  for (int x=x0; x<=x1; x++) {
    buffer.set(x, y, fillValue);
  }
}

template <class T>
void renderFromBuffer(const Buffer<T>& buffer, TGAImage& image) {
  for(int j=0; j<buffer.height; ++j) {
    for(int i=0; i<buffer.width; ++i) {
      image.set(i, j, buffer.get(i, j));
    }
  }
}

template void renderFromBuffer<TGAColor>(const Buffer<TGAColor>& buffer, TGAImage& image);

Vec3f getBarycentricCoords(const Vec2i& A, const Vec2i& B, const Vec2i& C, const Vec2i& P) {
  Vec3f u = Vec3f(B.x - A.x, C.x - A.x, A.x - P.x).cross(Vec3f(B.y - A.y, C.y - A.y, A.y - P.y));
  if (std::abs(u.z)<1) return Vec3f(-1,1,1); // triangle is degenerate, in this case return smth with negative coordinates
  return Vec3f(1.f-(u.x+u.y)/u.z, u.y/u.z, u.x/u.z);
}

template <class T>
void triangle(Vec2i *pts, Buffer<T> &buffer, T fillValue) {
  // Create bounding box
  Vec2i bboxmin(buffer.width-1, buffer.height-1);
  Vec2i bboxmax(0, 0);
  Vec2i clamp(buffer.width-1, buffer.height-1);
  for (int i=0; i<3; i++) {
    for (int j=0; j<2; j++) {
      // clip against buffer sides
      bboxmin[j] = std::max(0,        std::min(bboxmin[j], pts[i][j]));
      bboxmax[j] = std::min(clamp[j], std::max(bboxmax[j], pts[i][j]));
    }
  }

  // Check every pixel in bounding box
  Vec2i P;
  for (P.x=bboxmin.x; P.x<=bboxmax.x; ++P.x) {
    for (P.y=bboxmin.y; P.y<=bboxmax.y; ++P.y) {
      Vec3f bc_screen = getBarycentricCoords(pts[0], pts[1], pts[2], P);
      if (bc_screen.x<0 || bc_screen.y<0 || bc_screen.z<0) continue;
      buffer.set(P.x, P.y, fillValue);
    }
  }
}

template void triangle<int>(Vec2i *pts, Buffer<int> &idBuffer, int fillValue);

int main(int argc, char** argv) {
  Buffer<TGAColor> buffer(200, 200, black);
  Vec2i pts[3] = {Vec2i(10,10), Vec2i(100, 30), Vec2i(190, 160)};
  triangle(pts, buffer, red);

  TGAImage frame(200, 200, TGAImage::RGB);
  renderFromBuffer(buffer, frame);
  frame.flip_vertically(); // to place the origin in the bottom left corner of the image
  frame.write_tga_file("framebuffer.tga");
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

