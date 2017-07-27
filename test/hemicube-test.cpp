#include "catch.hpp"
#include "hemicube.hpp"
#include "colours.hpp"
#include "buffer.hpp"
#include "rendering.hpp"

TEST_CASE("Test formation of hemicube side face MVP", "[hemicube]") {
  Model model("test/scene.obj", "test/scene.mtl");

  int gridSize = 500;

  // Choose random face
  int faceIdx = 92;

  // Calc top face orientation
  Face f = model.face(faceIdx);
  Vec3f up = (model.vert(f[1].ivert) - model.vert(f[0].ivert)).normalise();
  Vec3f normal = calcNormal(model.vert(f[0].ivert), model.vert(f[1].ivert), model.vert(f[2].ivert));
  Vec3f dir = (normal*up.norm()).normalise();
  Vec3f eye = model.centreOf(faceIdx) + dir*0.01f;

  Matrix MVP = formHemicubeMVP(eye, dir, up, gridSize);
  Buffer<TGAColor> buffer(gridSize, gridSize, black);
  renderTestModelReflectivity(buffer, model, MVP);
  renderColourBuffer(buffer, "test/hemicubeMVP_top.tga");

  // Move into side face orientation
  std::swap(up, dir);

  MVP = formHemicubeMVP(eye, dir, up, gridSize);
  buffer.fillAll(black);
  renderTestModelReflectivity(buffer, model, MVP);
  renderColourBuffer(buffer, "test/hemicubeMVP_side1.tga");

  // Second side face orientation
  dir = dir*-1.f;
  MVP = formHemicubeMVP(eye, dir, up, gridSize);
  buffer.fillAll(black);
  renderTestModelReflectivity(buffer, model, MVP);
  renderColourBuffer(buffer, "test/hemicubeMVP_side2.tga");

  // Third
  dir = dir.cross(up);
  MVP = formHemicubeMVP(eye, dir, up, gridSize);
  buffer.fillAll(black);
  renderTestModelReflectivity(buffer, model, MVP);
  renderColourBuffer(buffer, "test/hemicubeMVP_side3.tga");

  // Fourth
  dir = dir*-1.f;
  MVP = formHemicubeMVP(eye, dir, up, gridSize);
  buffer.fillAll(black);
  renderTestModelReflectivity(buffer, model, MVP);
  renderColourBuffer(buffer, "test/hemicubeMVP_side4.tga");
}
