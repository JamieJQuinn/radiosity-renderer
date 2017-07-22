#include <iostream>

#include "rendering.hpp"
#include "tgaimage.hpp"
#include "geometry.hpp"
#include "buffer.hpp"
#include "model.hpp"
#include "colours.hpp"

void renderColourBuffer(const Buffer<TGAColor>& buffer, TGAImage& image) {
  for(int j=0; j<buffer.height; ++j) {
    for(int i=0; i<buffer.width; ++i) {
      image.set(i, j, buffer.get(i, j));
    }
  }
}

void renderColourBuffer(const Buffer<TGAColor>& buffer, std::string filename) {
  TGAImage frame(buffer.width, buffer.height, TGAImage::RGB);
  renderColourBuffer(buffer, frame);
  frame.flip_vertically(); // to place the origin in the bottom left corner of the image
  frame.write_tga_file(filename.c_str());
}

void renderZBuffer(const Buffer<float>& zBuffer, TGAImage& image) {
  float maxZ = zBuffer.max();
  for(int j=0; j<zBuffer.height; ++j) {
    for(int i=0; i<zBuffer.width; ++i) {
      float zVal = 255*zBuffer.get(i, j)/maxZ;
      TGAColor grey = TGAColor(zVal, zVal, zVal, 255);
      image.set(i, j, grey);
    }
  }
}

void renderZBuffer(const Buffer<float>& zBuffer, std::string filename) {
  TGAImage frame(zBuffer.width, zBuffer.height, TGAImage::RGB);
  renderZBuffer(zBuffer, frame);
  frame.flip_vertically(); // to place the origin in the bottom left corner of the image
  frame.write_tga_file(filename.c_str());
}

Vec3f getBarycentricCoords(const Vec3f& A, const Vec3f& B, const Vec3f& C, const Vec3f& P) {
  Vec3f u = Vec3f(B.x - A.x, C.x - A.x, A.x - P.x).cross(Vec3f(B.y - A.y, C.y - A.y, A.y - P.y));
  if (std::abs(u.z)<1) return Vec3f(-1,1,1); // triangle is degenerate, in this case return smth with negative coordinates
  return Vec3f(1.f-(u.x+u.y)/u.z, u.x/u.z, u.y/u.z);
}

void renderWireFrame(const Model& model, Buffer<TGAColor>& buffer, const Matrix& MVP) {
  for (int i=0; i<model.nfaces(); i++) {
    std::vector<Vec3i> face = model.face(i);
    std::swap(face[0], face[1]);
    int nVerts = face.size();
    for (int j=0; j<nVerts; j++) {
      Vec3f v0 = m2v(MVP*v2m(model.vert(face[j].ivert)));
      Vec3f v1 = m2v(MVP*v2m(model.vert(face[(j+1)%nVerts].ivert)));
      renderLine(v0.x, v0.y, v1.x, v1.y, buffer, white);
    }
  }
}

Matrix viewportRelative(int x, int y, int w, int h, int depth) {
  Matrix m = Matrix::identity(4);
  m.set(0, 3, x+w/2.f);
  m.set(1, 3, y+h/2.f);
  m.set(2, 3, depth/2.f);

  m.set(0, 0, w/2.f);
  m.set(1, 1, h/2.f);
  m.set(2, 2, depth/2.f);
  return m;
}

Matrix viewportAbsolute(int x0, int y0, int x1, int y1, int depth) {
  return viewportRelative(x0, y0, x1-x0, y1-y0, depth);
}

Matrix lookAt(Vec3f eye, Vec3f centre, Vec3f up) {
  Vec3f z = (eye-centre).normalise();
  Vec3f x = up.cross(z).normalise();
  Vec3f y = z.cross(x).normalise();
  Matrix Minv = Matrix::identity();
  Matrix Tr   = Matrix::identity();
  for (int i=0; i<3; i++) {
    Minv.set(0, i, x[i]);
    Minv.set(1, i, y[i]);
    Minv.set(2, i, z[i]);
    Tr.set(i, 3, -centre[i]);
  }
  return Minv*Tr;
}
