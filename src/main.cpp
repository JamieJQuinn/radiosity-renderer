#include <iostream>

#include "buffer.hpp"
#include "tgaimage.hpp"
#include "model.hpp"
#include "geometry.hpp"
#include "rendering.hpp"

int main(int argc, char** argv) {
  const TGAColor black   = TGAColor(0, 0,   0,   255);
  int size = 800;

  Model model("data/scene.obj");

  Vec3f camera(0,0,5);
  Matrix P = Matrix::identity(4);
  P.set(3, 2, -1.f/camera.z);
  Matrix V = viewportAbsolute(3*size/8, 3*size/8, 5*size/8, 5*size/8, 255);
  Matrix MVP = V*P;
  Buffer<TGAColor> buffer(size, size, black);

  renderWireFrame(model, buffer, MVP);

  TGAImage image(size, size, TGAImage::RGB);
  renderColourBuffer(buffer, image);
  image.flip_vertically();
  image.write_tga_file("output.tga");

  return 0;
}
