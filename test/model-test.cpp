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
  //verifyModelNormalsConsistent("test/scene_subdivided.obj", "test/scene_subdivided.mtl");
  verifyModelNormalsConsistent("test/simple_box.obj", "test/simple_box.mtl");
  //verifyModelNormalsConsistent("test/simple_box_subdivided.obj", "test/simple_box_subdivided.mtl");
  verifyModelNormalsConsistent("test/dual_cube_different_normals.obj", "test/dual_cube_different_normals.mtl");
}

TEST_CASE("Test loading of materials", "[model]") {
  Model model("test/scene_subdivided.obj", "test/scene_subdivided.mtl");

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

  renderColourBuffer(buffer, "test/scene_subdivided_inside.tga");
}

TEST_CASE("Test looking at scene from inside", "[model]") {
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
  Model model("test/simple_box_subdivided.obj", "test/simple_box_subdivided.mtl");

  Vec3f eye(-1.5, -1, -3);
  Vec3f dir = eye*-1;
  Vec3f up(0, 0, 1);
  int size = 800;
  Matrix MVP = formHemicubeMVP(eye, dir, up, size);

  Buffer<TGAColor> buffer(size, size, black);
  renderTestModelReflectivity(buffer, model, MVP);

  renderColourBuffer(buffer, "test/simple_box_subdivided_inside_normals.tga");
}

TEST_CASE("Test viewing subdivided scene", "[model]") {
  Model model("test/scene_subdivided.obj", "test/scene_subdivided.mtl");

  Vec3f eye(-4, -5, 6);
  Vec3f dir = eye*-1;
  Vec3f up(0, 0, 1);
  int size = 800;
  Matrix MVP = formHemicubeMVP(eye, dir, up, size);

  Buffer<TGAColor> buffer(size, size, black);
  renderTestModelReflectivity(buffer, model, MVP);

  renderColourBuffer(buffer, "test/scene_subdivided_outside.tga");
}

TEST_CASE("Test viewing simple scene", "[model]") {
  Model model("test/scene.obj", "test/scene.mtl");

  Vec3f eye(-4, -5, 6);
  Vec3f dir = eye*-1;
  Vec3f up(0, 0, 1);
  int size = 800;
  Matrix MVP = formHemicubeMVP(eye, dir, up, size);

  Buffer<TGAColor> buffer(size, size, black);
  renderTestModelReflectivity(buffer, model, MVP);

  renderColourBuffer(buffer, "test/scene_outside.tga");
}

TEST_CASE("Test viewing two boxes with different normals", "[model]") {
  Model model("test/dual_cube_different_normals.obj", "test/dual_cube_different_normals.mtl");

  Vec3f eye(2, 4, 3);
  Vec3f dir = eye*-1;
  Vec3f up(0, 0, 1);
  int size = 800;
  Matrix MVP = formHemicubeMVP(eye, dir, up, size);

  Buffer<TGAColor> buffer(size, size, black);
  renderTestModelReflectivity(buffer, model, MVP);

  renderColourBuffer(buffer, "test/dual_cube_different_normals.tga");
}
