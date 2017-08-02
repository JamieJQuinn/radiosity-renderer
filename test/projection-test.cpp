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
  Model model("test/simple_box.obj", "test/simple_box.mtl");

  Vec3f eye(2, 2.5, 3);
  Vec3f dir = eye*-1;
  Vec3f up(0, 0, 1);
  int size = 800;
  Matrix MVP = formHemicubeMVP(eye, dir, up, size);

  Buffer<TGAColor> buffer(size, size, black);
  renderWireFrame(model, buffer, MVP);

  renderColourBuffer(buffer, "test/moved_camera.tga");
}

TEST_CASE("Ensure MVP retains z order", "[projection]") {
  Vec3f eye(2, 2.5, 3);
  Vec3f dir = (eye*-1).normalise();
  Vec3f up(0, 0, 1);
  int size = 800;

  Vec3f v(1,1,1);
  Vec3f w(-1,-1,-1);

  Matrix translation = formTranslation(eye*-1);
  Matrix view = lookAt(Vec3f(0, 0, 0), dir, up)*translation;
  Vec3f vAfterView = applyTransform(view, v);
  Vec3f wAfterView = applyTransform(view, w);
  REQUIRE(vAfterView.z > wAfterView.z);

  Matrix projection = formRightAngledProjection(0.05f, 1.0f);
  Vec3f vAfterProjection = applyTransform(projection, vAfterView);
  Vec3f wAfterProjection = applyTransform(projection, wAfterView);
  REQUIRE(vAfterProjection.z > wAfterProjection.z);

  Matrix viewport = viewportRelative(0, 0, size, size);
  Vec3f vAfterViewport = applyTransform(viewport, vAfterProjection);
  Vec3f wAfterViewport = applyTransform(viewport, wAfterProjection);
  REQUIRE(vAfterViewport.z > wAfterViewport.z);
}

TEST_CASE("Test moving the camera (filled triangles)", "[camera]") {
  Model model("test/simple_box.obj", "test/simple_box.mtl");

  Vec3f eye(2, 2.5, 3);
  Vec3f dir = eye*-1;
  Vec3f up(0, 0, 1);
  int size = 800;
  Matrix MVP = formHemicubeMVP(eye, dir, up, size);

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
