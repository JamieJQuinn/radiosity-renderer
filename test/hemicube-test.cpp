#include "catch.hpp"
#include "hemicube.hpp"
#include "colours.hpp"
#include "buffer.hpp"
#include "rendering.hpp"

TEST_CASE("Test formation of hemicube side face MVP", "[hemicube]") {
  Model model("test/simple_box_subdivided.obj", "test/simple_box_subdivided.mtl");

  int gridSize = 500;

  // Choose random face
  int faceIdx = 0;

  Buffer<TGAColor> mainBuffer(gridSize*2, gridSize*2, black);

  // Calc top face orientation
  Face f = model.face(faceIdx);
  //Vec3f up = (model.vert(f[1].ivert) - model.vert(f[0].ivert)).normalise();
  //Vec3f normal = calcNormal(model.vert(f[0].ivert), model.vert(f[1].ivert), model.vert(f[2].ivert));
  //Vec3f dir = (normal*up.norm()).normalise();
  //Vec3f dir = model.norm(faceIdx, 0);
  Vec3f dir = Vec3f(0,1,0);
  //Vec3f eye = model.centreOf(faceIdx) + dir*0.01f;

  Vec3f up(0,0,1);
  Vec3f eye(0.2f,-0.1f,0.3f);

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

  renderColourBuffer(buffer, "test/hemicubeMVP_up.tga");

  for(int j=0; j<gridSize/2; ++j) {
    for(int i=0; i<gridSize; ++i) {
      mainBuffer.set(gridSize/2+i, j+3*gridSize/2, buffer.get(i, j));
    }
  }

  // Second side face orientation
  dir = dir*-1.f;
  MVP = formHemicubeMVP(eye, dir, up, gridSize);
  buffer.fillAll(black);
  renderTestModelReflectivity(buffer, model, MVP);

  renderColourBuffer(buffer, "test/hemicubeMVP_down.tga");

  for(int j=0; j<gridSize/2; ++j) {
    for(int i=0; i<gridSize; ++i) {
      mainBuffer.set(gridSize/2+i, j, buffer.get(i, j+gridSize/2));
    }
  }

  // Third
  dir = dir.cross(up);
  MVP = formHemicubeMVP(eye, dir, up, gridSize);
  buffer.fillAll(black);
  renderTestModelReflectivity(buffer, model, MVP);

  renderColourBuffer(buffer, "test/hemicubeMVP_right.tga");

  for(int j=0; j<gridSize/2; ++j) {
    for(int i=0; i<gridSize; ++i) {
      mainBuffer.set(j+3*gridSize/2, i+gridSize/2, buffer.get(i, j));
    }
  }

  // Fourth
  dir = dir*-1.f;
  MVP = formHemicubeMVP(eye, dir, up, gridSize);
  buffer.fillAll(black);
  renderTestModelReflectivity(buffer, model, MVP);

  renderColourBuffer(buffer, "test/hemicubeMVP_left.tga");

  for(int j=0; j<gridSize/2; ++j) {
    for(int i=0; i<gridSize; ++i) {
      mainBuffer.set(j, i+gridSize/2, buffer.get(gridSize-i, gridSize/2+j));
    }
  }

  renderColourBuffer(mainBuffer, "test/hemicubeMVP.tga");
}