#include "catch.hpp"
#include "buffer.hpp"
#include "geometry.hpp"
#include "model.hpp"
#include "tgaimage.hpp"
#include "rendering.hpp"

TEST_CASE("Test orthographic projection", "[projection]") {
  const TGAColor black = TGAColor(0, 0, 0, 255);
  int size = 800;

  Model model("test/simple_box.obj");

  Matrix MVP = viewportRelative(size/4, size/4, size/2, size/2, 255);
  Buffer<TGAColor> buffer(size, size, black);

  renderWireFrame(model, buffer, MVP);

  renderColourBuffer(buffer, "test/orthographic_projection.tga");
}

TEST_CASE("Test perspective projection", "[projection]") {
  const TGAColor black = TGAColor(0, 0, 0, 255);
  int size = 800;

  Model model("test/simple_box.obj");

  Vec3f camera(0,0,3);
  Matrix P = Matrix::identity(4);
  P.set(3, 2, -1.f/camera.z);
  Matrix V = viewportRelative(size/4, size/4, size/2, size/2, 255);
  Matrix MVP = V*P;
  Buffer<TGAColor> buffer(size, size, black);

  renderWireFrame(model, buffer, MVP);

  renderColourBuffer(buffer, "test/perspective_projection.tga");
}
