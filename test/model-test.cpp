#include "catch.hpp"
#include "model.hpp"
#include "tgaimage.hpp"
#include "geometry.hpp"
#include "rendering.hpp"
#include "colours.hpp"
#include "hemicube.hpp"

TEST_CASE("Test loading of materials", "[model]") {
  Model model("test/scene.obj", "test/scene.mtl");

  int gridSize = 500;

  // Choose random face
  int faceIdx = 0;

  Face f = model.face(faceIdx);
  Vec3f up = model.vert(f[1].ivert) - model.vert(f[0].ivert);
  Vec3f dir = model.norm(faceIdx, 0)*up.norm();
  Vec3f eye = model.centreOf(faceIdx) + dir*0.01f;

  Matrix MVP = formHemicubeMVP(eye, dir, up, gridSize);

  Buffer<TGAColor> buffer(gridSize, gridSize, black);

  renderTestModelReflectivity(buffer, model, MVP);

  renderColourBuffer(buffer, "test/scene_inside.tga");
}

TEST_CASE("Test viewing subdivided model", "[model]") {
  int size = 800;

  Model model("test/simple_box_subdivided.obj", "test/simple_box_subdivided.mtl");

  Vec3f eye(-2, -2.5, -3);
  Vec3f centre(0,0,0);
  Vec3f up(0, 1, 0);
  Matrix modelView = lookAt(eye, centre, up);

  Matrix P = Matrix::identity(4);
  P.set(3, 2, -1.f/(eye-centre).norm());
  Matrix V = viewportRelative(size/4, size/4, size/2, size/2);
  Matrix MVP = V*P*modelView;

  Buffer<TGAColor> buffer(size, size, black);
  renderTestModel(buffer, model, MVP);

  renderColourBuffer(buffer, "test/simple_box_subdivided_filled.tga");
}
