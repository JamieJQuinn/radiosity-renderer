#include "catch.hpp"
#include "hemicube.hpp"
#include "colours.hpp"
#include "buffer.hpp"
#include "rendering.hpp"

TEST_CASE("Test formation of hemicube side face MVP", "[hemicube]") {
  Model model("test/scene.obj", "test/scene.mtl");

  int gridSize = 500;

  // Choose random face
  int faceIdx = 3;

  Buffer<TGAColor> mainBuffer(gridSize*2, gridSize*2, black);

  // Calc top face orientation
  Face f = model.face(faceIdx);
  Vec3f up = (model.vert(f[1].ivert) - model.vert(f[0].ivert)).normalise();
  Vec3f normal = calcNormal(model.vert(f[0].ivert), model.vert(f[1].ivert), model.vert(f[2].ivert));
  Vec3f dir = (normal*-up.norm()).normalise();
  Vec3f eye = model.centreOf(faceIdx) + dir*0.001f;

  Matrix MVP = formHemicubeMVP(eye, dir, up, gridSize);
  Buffer<TGAColor> buffer(gridSize, gridSize, black);
  renderTestModelReflectivity(buffer, model, MVP);

  for(int j=0; j<gridSize; ++j) {
    for(int i=0; i<gridSize; ++i) {
      mainBuffer.set(gridSize/2+i, gridSize/2+j, buffer.get(i, j));
    }
  }

  // Move into side face orientation
  std::swap(up, dir);
  MVP = formHemicubeMVP(eye, dir, up, gridSize);
  buffer.fillAll(black);
  renderTestModelReflectivity(buffer, model, MVP);

  for(int j=0; j<gridSize/2; ++j) {
    for(int i=0; i<gridSize; ++i) {
      mainBuffer.set(gridSize/2+i, j, buffer.get(gridSize-i, j));
    }
  }

  // Second side face orientation
  dir = dir*-1.f;
  MVP = formHemicubeMVP(eye, dir, up, gridSize);
  buffer.fillAll(black);
  renderTestModelReflectivity(buffer, model, MVP);

  for(int j=0; j<gridSize/2; ++j) {
    for(int i=0; i<gridSize; ++i) {
      mainBuffer.set(gridSize/2+i, j+3*gridSize/2, buffer.get(i, j));
    }
  }

  // Third
  dir = dir.cross(up);
  MVP = formHemicubeMVP(eye, dir, up, gridSize);
  buffer.fillAll(black);
  renderTestModelReflectivity(buffer, model, MVP);

  for(int j=0; j<gridSize/2; ++j) {
    for(int i=0; i<gridSize; ++i) {
      mainBuffer.set(j, i+gridSize/2, buffer.get(i, j));
    }
  }

  // Fourth
  dir = dir*-1.f;
  MVP = formHemicubeMVP(eye, dir, up, gridSize);
  buffer.fillAll(black);
  renderTestModelReflectivity(buffer, model, MVP);

  for(int j=0; j<gridSize/2; ++j) {
    for(int i=0; i<gridSize; ++i) {
      mainBuffer.set(j+3*gridSize/2, i+gridSize/2, buffer.get(gridSize-i, j));
    }
  }

  renderColourBuffer(mainBuffer, "test/hemicubeMVP.tga");
}
