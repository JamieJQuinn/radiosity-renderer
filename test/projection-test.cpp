#include "catch.hpp"
#include "buffer.hpp"
#include "geometry.hpp"
#include "model.hpp"
#include "tgaimage.hpp"
#include "rendering.hpp"
#include "colours.hpp"
#include "hemicube.hpp"

TEST_CASE("Test perspective projection", "[projection]") {
  int size = 800;

  Model model("test/simple_box.obj", "test/simple_box.mtl");

  Vec3f camera(0,0,3);
  Matrix P = Matrix::identity(4);
  P.set(3, 2, -1.f/camera.z);
  Matrix V = viewportRelative(size/4, size/4, size/2, size/2);
  Matrix MVP = V*P;
  Buffer<TGAColor> buffer(size, size, black);

  renderWireFrame(model, buffer, MVP);

  renderColourBuffer(buffer, "test/perspective_projection.tga");
}

TEST_CASE("Test moving the camera (perspective)", "[camera]") {
  int size = 800;

  Model model("test/simple_box.obj", "test/simple_box.mtl");

  Vec3f eye(2, 2.5, 3);
  Vec3f centre(0,0,0);
  Vec3f up(0, 0, 1);
  Matrix modelView = lookAt(eye, centre, up);

  Matrix P = Matrix::identity(4);
  P.set(3, 2, -1.f/(eye-centre).norm());
  Matrix V = viewportRelative(size/4, size/4, size/2, size/2);
  Matrix MVP = V*P*modelView;

  Buffer<TGAColor> buffer(size, size, black);
  renderWireFrame(model, buffer, MVP);

  renderColourBuffer(buffer, "test/moved_camera.tga");
}

TEST_CASE("Test moving the camera (filled triangles)", "[camera]") {
  int size = 800;

  Model model("test/simple_box.obj", "test/simple_box.mtl");

  Vec3f eye(2, 2.5, 3);
  Vec3f centre(0,0,0);
  Vec3f up(0, 0, 1);
  Matrix modelView = lookAt(eye, centre, up);

  Matrix P = Matrix::identity(4);
  P.set(3, 2, -1.f/(eye-centre).norm());
  Matrix V = viewportRelative(size/4, size/4, size/2, size/2);
  Matrix MVP = V*P*modelView;

  Buffer<TGAColor> buffer(size, size, black);

  renderTestModelReflectivity(buffer, model, MVP);

  renderColourBuffer(buffer, "test/simple_box_filled.tga");
}

TEST_CASE("Test viewing subdivided model from inside (filled triangles)", "[camera]") {
  Model model("test/simple_box_subdivided.obj", "test/simple_box_subdivided.mtl");

  Vec3f dir = Vec3f(0,1,0);
  Vec3f eye(0.2f, -.5f, 0.3f);
  Vec3f up(0, 0, 1);
  int size = 800;
  Matrix MVP = formHemicubeMVP(eye, dir, up, size);

  Buffer<TGAColor> buffer(size, size, black);

  renderTestModelReflectivity(buffer, model, MVP);

  renderColourBuffer(buffer, "test/simple_box_subdivided_inside.tga");
}
