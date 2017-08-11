#include "catch.hpp"
#include "model.hpp"
#include "tgaimage.hpp"
#include "geometry.hpp"
#include "rendering.hpp"
#include "colours.hpp"
#include "hemicube.hpp"

void verifyModelNormalsConsistent(std::string objFilename, std::string mtlFilename) {
  Model model(objFilename.c_str(), mtlFilename.c_str());

  for (int i=0; i<model.nfaces(); ++i) {
    Face face = model.face(i);
    Vec3f world_coords[3];
    for (int j=0; j<3; j++) {
      world_coords[j] = model.vert(face[j].ivert);
    }
    Vec3f n = calcNormal(world_coords[0], world_coords[1], world_coords[2]);
    for(int j=0; j<3; ++j) {
      REQUIRE(n[j] == Approx(model.norm(i, 0)[j]));
    }
  }
}

TEST_CASE("Test model normals are same as normals calculated from winding", "[model]") {
  verifyModelNormalsConsistent("test/scene.obj", "test/scene.mtl");
  verifyModelNormalsConsistent("test/simple_box.obj", "test/simple_box.mtl");
  verifyModelNormalsConsistent("test/dual_cube_different_normals.obj", "test/dual_cube_different_normals.mtl");
}

TEST_CASE("Test viewing subdivided scene", "[model]") {
  Model model("test/scene_subdivided.obj", "test/scene_subdivided.mtl");

  Vec3f eye(-3, -4, 5);
  Vec3f dir = eye*-1;
  Vec3f up(0, 0, 1);
  int size = 800;
  Matrix MVP = formHemicubeMVP(eye, dir, up);

  Buffer<TGAColor> buffer(size, size, black);
  renderModelReflectivity(buffer, model, MVP, eye, 0.05f);

  renderColourBuffer(buffer, "test/scene_outside.tga");
}

TEST_CASE("Test viewing two boxes with different normals", "[model]") {
  Model model("test/dual_cube_different_normals.obj", "test/dual_cube_different_normals.mtl");

  Vec3f eye(2, 4, 3);
  Vec3f dir = eye*-1;
  Vec3f up(0, 0, 1);
  int size = 800;
  Matrix MVP = formHemicubeMVP(eye, dir, up);

  Buffer<TGAColor> buffer(size, size, black);
  renderModelReflectivity(buffer, model, MVP, eye, 0.05f);

  renderColourBuffer(buffer, "test/dual_cube_different_normals.tga");
}
