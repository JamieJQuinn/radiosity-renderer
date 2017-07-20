#include <iostream>

#include "buffer.hpp"
#include "tgaimage.hpp"
#include "model.hpp"
#include "geometry.hpp"
#include "rendering.hpp"

template void renderLine(int, int, int, int, Buffer<TGAColor>&, const TGAColor&);
template void renderTriangle(Vec3f*, Buffer<float>&, Buffer<TGAColor>&, const TGAColor&);

void testZBuffer() {
  const TGAColor white = TGAColor(255, 255, 255, 255);
  const TGAColor red   = TGAColor(255, 0,   0,   255);
  Buffer<TGAColor> buffer(200, 200, white);
  Buffer<float> zBuffer(200, 200, 0);
  Vec3f pts[3] = {Vec3f(10,10, 244), Vec3f(100, 30, 0), Vec3f(190, 160, 100)};
  renderTriangle(pts, zBuffer, buffer, red);

  TGAImage frame(200, 200, TGAImage::RGB);
  //renderColourBuffer(buffer, frame);
  renderZBuffer(zBuffer, frame);
  frame.flip_vertically(); // to place the origin in the bottom left corner of the image
  frame.write_tga_file("output.tga");
}

void testWireFrame() {
  const TGAColor black   = TGAColor(0, 0,   0,   255);
  const TGAColor red   = TGAColor(255, 0,   0,   255);
  const TGAColor green   = TGAColor(0, 255,   0,   255);
  int size = 800;
  Model* model = new Model("test/simple_box.obj");

  Buffer<TGAColor> buffer(size, size, black);
  renderWireFrame(model, buffer, size, size);
  renderLine(size/2, size/2, 3*size/4, size/2, buffer, red);
  renderLine(size/2, size/2, size/2, 3*size/4, buffer, green);

  TGAImage image(size, size, TGAImage::RGB);
  renderColourBuffer(buffer, image);
  image.flip_vertically();
  image.write_tga_file("output.tga");
  delete model;
}



//Matrix viewport(int x, int y, int w, int h, int depth) {
  //Matrix m = Matrix::identity(4);
  //m[0][3] = x+w/2.f;
  //m[1][3] = y+h/2.f;
  //m[2][3] = depth/2.f;

  //m[0][0] = w/2.f;
  //m[1][1] = h/2.f;
  //m[2][2] = depth/2.f;
  //return m;
//}

//void testPerspective() {
  //Vec3f camera(0,0,3);

  //const TGAColor black   = TGAColor(0, 0,   0,   255);
  //const TGAColor red   = TGAColor(255, 0,   0,   255);
  //const TGAColor green   = TGAColor(0, 255,   0,   255);
  //int size = 800;
  //Model* model = new Model("test/simple_box.obj");

  //Buffer<TGAColor> buffer(size, size, black);
  //renderWireFrame(model, buffer, size, size);
  //renderLine(size/2, size/2, 3*size/4, size/2, buffer, red);
  //renderLine(size/2, size/2, size/2, 3*size/4, buffer, green);

  //TGAImage image(size, size, TGAImage::RGB);
  //renderColourBuffer(buffer, image);
  //image.flip_vertically();
  //image.write_tga_file("output.tga");
  //delete model;
//}

int main(int argc, char** argv) {
  testWireFrame();

  return 0;
}
