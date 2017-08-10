#include "catch.hpp"
#include "hemicube.hpp"
#include "colours.hpp"
#include "buffer.hpp"
#include "rendering.hpp"

TEST_CASE("Test formation of hemicube side face MVP", "[hemicube]") {
  Model model("test/scene.obj", "test/scene.mtl");

  int gridSize = 500;

  // Choose random face
  int faceIdx = 0;

  Buffer<TGAColor> mainBuffer(gridSize*2, gridSize*2, black);

  // Calc top face orientation
  Face f = model.face(faceIdx);
  Vec3f up = (model.vert(f[1].ivert) - model.vert(f[0].ivert)).normalise();
  Vec3f dir = model.norm(faceIdx, 0);
  Vec3f eye = model.centreOf(faceIdx);

  Matrix MVP = formHemicubeMVP(eye, dir, up);
  Buffer<TGAColor> buffer(gridSize, gridSize, black);
  renderModelReflectivity(buffer, model, MVP, dir, 0.05f);

  for(int j=0; j<gridSize; ++j) {
    for(int i=0; i<gridSize; ++i) {
      mainBuffer.set(gridSize/2+i, gridSize/2+j, buffer.get(i, j));
    }
  }

  // Move into side face orientation
  std::swap(up, dir);
  MVP = formHemicubeMVP(eye, dir, up);
  buffer.fillAll(black);
  renderModelReflectivity(buffer, model, MVP, dir, 0.05f);

  for(int j=0; j<gridSize/2; ++j) {
    for(int i=0; i<gridSize; ++i) {
      mainBuffer.set(gridSize/2+i, j+3*gridSize/2, buffer.get(gridSize-i, j+gridSize/2));
    }
  }

  // Second side face orientation
  dir = dir*-1.f;
  MVP = formHemicubeMVP(eye, dir, up);
  buffer.fillAll(black);
  renderModelReflectivity(buffer, model, MVP, dir, 0.05f);

  for(int j=0; j<gridSize/2; ++j) {
    for(int i=0; i<gridSize; ++i) {
      mainBuffer.set(gridSize/2+i, j, buffer.get(i, j+gridSize/2));
    }
  }

  // Third
  dir = dir.cross(up);
  MVP = formHemicubeMVP(eye, dir, up);
  buffer.fillAll(black);
  renderModelReflectivity(buffer, model, MVP, dir, 0.05f);

  for(int j=0; j<gridSize/2; ++j) {
    for(int i=0; i<gridSize; ++i) {
      mainBuffer.set(j+3*gridSize/2, i+gridSize/2, buffer.get(i, j+gridSize/2));
    }
  }

  // Fourth
  dir = dir*-1.f;
  MVP = formHemicubeMVP(eye, dir, up);
  buffer.fillAll(black);
  renderModelReflectivity(buffer, model, MVP, dir, 0.05f);

  for(int j=0; j<gridSize/2; ++j) {
    for(int i=0; i<gridSize; ++i) {
      mainBuffer.set(j, i+gridSize/2, buffer.get(gridSize-i, gridSize/2+j));
    }
  }

  renderColourBuffer(mainBuffer, "test/hemicubeMVP.tga");
}

TEST_CASE("Render single hemicube face to ID index", "[hemicube]") {
  Model model("test/scene.obj", "test/scene.mtl");
  int nFaces = model.nfaces() + 1;
  int faceIdx = 0;
  int gridSize = 100;
  Buffer<int> buffer(gridSize, gridSize, 0);
  renderHemicubeFront(buffer, model, faceIdx);

  for(int j=0; j<buffer.height; ++j) {
    for(int i=0; i<buffer.width; ++i) {
      REQUIRE(buffer.get(i, j) < nFaces);
    }
  }

  renderIdsToColour(buffer, model, "test/hemicube_face_IDs.tga");
}

TEST_CASE("Render hemicube to ID index", "[hemicube]") {
  Model model("test/scene.obj", "test/scene.mtl");
  int nFaces = model.nfaces() + 1;
  int faceIdx = 0;
  int gridSize = 100;
  Buffer<int> mainBuffer(gridSize*2, gridSize*2, 0);

  renderHemicubeFront(mainBuffer, model, faceIdx);
  for(int j=0; j<mainBuffer.height; ++j) {
    for(int i=0; i<mainBuffer.width; ++i) {
      REQUIRE(mainBuffer.get(i, j) < nFaces);
    }
  }

  renderHemicubeUp(mainBuffer, model, faceIdx);
  for(int j=0; j<mainBuffer.height; ++j) {
    for(int i=0; i<mainBuffer.width; ++i) {
      REQUIRE(mainBuffer.get(i, j) < nFaces);
    }
  }

  renderHemicubeDown(mainBuffer, model, faceIdx);
  for(int j=0; j<mainBuffer.height; ++j) {
    for(int i=0; i<mainBuffer.width; ++i) {
      REQUIRE(mainBuffer.get(i, j) < nFaces);
    }
  }

  renderHemicubeLeft(mainBuffer, model, faceIdx);
  for(int j=0; j<mainBuffer.height; ++j) {
    for(int i=0; i<mainBuffer.width; ++i) {
      REQUIRE(mainBuffer.get(i, j) < nFaces);
    }
  }

  renderHemicubeRight(mainBuffer, model, faceIdx);
  for(int j=0; j<mainBuffer.height; ++j) {
    for(int i=0; i<mainBuffer.width; ++i) {
      REQUIRE(mainBuffer.get(i, j) < nFaces);
    }
  }

  renderIdsToColour(mainBuffer, model, "test/hemicubeIDs.tga");
}
