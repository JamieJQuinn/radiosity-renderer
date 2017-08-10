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
  int faceIdx = 8;
  int gridSize = 100;
  Buffer<int> mainBuffer(gridSize*2, gridSize*2, 0);

  renderToHemicube(mainBuffer, model, faceIdx);
  for(int j=0; j<mainBuffer.height; ++j) {
    for(int i=0; i<mainBuffer.width; ++i) {
      REQUIRE(mainBuffer.get(i, j) < nFaces);
    }
  }

  renderIdsToColour(mainBuffer, model, "test/hemicubeIDs.tga");
}

TEST_CASE("Calculate form factors", "[hemicube]") {
  Model model("test/scene.obj", "test/scene.mtl");
  int faceIdx = 0;
  int gridSize = 100;
  std::vector<float> formFactors(model.nfaces()+1);

  calcFormFactorsFromModel(model, faceIdx, formFactors, gridSize);

  float sum = 0;
  for(int i=1; i<(int)formFactors.size(); ++i) {
    sum += formFactors[i];
  }
  // sum should be as close to 1 as possible
  REQUIRE(sum > 0.95f);
}

TEST_CASE("Calculate radiosity from one patch", "[radiosity]") {
  Model model("test/scene.obj", "test/scene.mtl");
  int faceIdx = 12;
  int gridSize = 100;

  std::vector<float> formFactors(model.nfaces()+1);
  calcFormFactorsFromModel(model, faceIdx, formFactors, gridSize);

  std::vector<Vec3f> radiosity(model.nfaces());
  std::vector<Vec3f> radiosityToShoot(model.nfaces());
  for(int i=0; i<model.nfaces(); ++i) {
    radiosityToShoot[i] = model.getFaceEmissivity(i);
    radiosity[i] = model.getFaceEmissivity(i);
  }

  shootRadiosity(model, gridSize, radiosity, radiosityToShoot, faceIdx, formFactors);
}

TEST_CASE("Render radiosity simple scene (single pass)", "[radiosity]") {
  Model model("test/scene.obj", "test/scene.mtl");
  int gridSize = 100;

  // Setup radiosity
  std::vector<Vec3f> radiosity(model.nfaces());
  std::vector<Vec3f> radiosityToShoot(model.nfaces());
  for(int i=0; i<model.nfaces(); ++i) {
    radiosityToShoot[i] = model.getFaceEmissivity(i);
    radiosity[i] = model.getFaceEmissivity(i);
  }

  std::vector<float> formFactors(model.nfaces()+1);

  for(int i=0; i<model.nfaces(); ++i) {
    calcFormFactorsFromModel(model, i, formFactors, gridSize);
    shootRadiosity(model, gridSize, radiosity, radiosityToShoot, i, formFactors);
  }

  Vec3f eye(-4, -5, 6);
  Vec3f dir = eye*-1;
  Vec3f up(0, 0, 1);
  int size = 800;
  float nearPlane = 0.05f;
  Matrix MVP = formHemicubeMVP(eye, dir, up);

  Buffer<TGAColor> buffer(size, size, black);
  renderModelRadiosity(buffer, model, MVP, dir, nearPlane, radiosity);

  renderColourBuffer(buffer, "test/radiosity_single_pass.tga");
}

TEST_CASE("Render radiosity over complex scene", "[radiosity]") {
  Model model("test/scene_subdivided.obj", "test/scene_subdivided.mtl");
  int gridSize = 100;

  // Setup radiosity
  std::vector<Vec3f> radiosity(model.nfaces());
  std::vector<Vec3f> radiosityToShoot(model.nfaces());
  for(int i=0; i<model.nfaces(); ++i) {
    radiosityToShoot[i] = model.getFaceEmissivity(i)*0.5f;
    radiosity[i] = model.getFaceEmissivity(i)*0.5f;
  }

  std::vector<float> formFactors(model.nfaces()+1);

  for(int passes=0; passes<2; ++passes) {
    for(int i=0; i<model.nfaces(); ++i) {
      calcFormFactorsFromModel(model, i, formFactors, gridSize);
      shootRadiosity(model, gridSize, radiosity, radiosityToShoot, i, formFactors);
    }
  }

  // Normalise radiosity
  float max = 0.f;
  for(int i=0; i<(int)radiosity.size(); ++i) {
    for(int j=0; j<3; ++j) {
      max = radiosity[i][j] > max ? radiosity[i][j] : max;
    }
  }
  for(int i=0; i<(int)radiosity.size(); ++i) {
    radiosity[i] = radiosity[i] * (1.f/max);
  }

  Vec3f eye(-4, -5, 6);
  Vec3f dir = eye*-1;
  Vec3f up(0, 0, 1);
  int size = 800;
  float nearPlane = 0.05f;
  Matrix MVP = formHemicubeMVP(eye, dir, up);

  Buffer<TGAColor> buffer(size, size, black);
  renderModelRadiosity(buffer, model, MVP, dir, nearPlane, radiosity);

  renderColourBuffer(buffer, "test/radiosity_complex.tga");
}
