#include "catch.hpp"
#include "hemicube.hpp"
#include "colours.hpp"
#include "buffer.hpp"
#include "rendering.hpp"

void renderViewFromFace(int faceIdx, int gridSize, const Model& model, std::string filename) {
  Buffer<int> buffer(gridSize, gridSize, 0);
  renderToHemicube(buffer, model, faceIdx);
  renderIdsToColour(buffer, model, filename);
}

TEST_CASE("Test two adjacent faces hemicube", "[hemicube]") {
  Model model("test/scene.obj", "test/scene.mtl");
  int gridSize = 200;
  int faceIdx = 0;
  renderViewFromFace(faceIdx, gridSize, model, "test/hemicubeFromFace1.tga");
  faceIdx = 7;
  renderViewFromFace(faceIdx, gridSize, model, "test/hemicubeFromFace2.tga");
}

//TEST_CASE("Render whole scene hemicubes", "[hemicube]") {
  //Model model("test/scene_subdivided.obj", "test/scene_subdivided.mtl");
  //int gridSize = 200;
  //Buffer<int> buffer(gridSize, gridSize, 0);

  //for(int faceIdx=0; faceIdx<model.nfaces(); ++faceIdx) {
    //std::stringstream s;
    //s << "test/hemicube_scene/" << faceIdx << ".tga";
    //renderToHemicube(buffer, model, faceIdx);
    //renderIdsToColour(buffer, model, s.str());
  //}
//}

TEST_CASE("Test two form factor of 2 different orientations of triangle", "[formFactor]") {
  int size = 20;
  Buffer<int> buffer(size, size, 0);
  Buffer<float> zBuffer(size, size, 0);
  std::vector<Vec3f> pts1({Vec3f(12, 2, 1), Vec3f(5, 16, 1), Vec3f(16, 10, 1)});
  std::vector<Vec3f> pts2({Vec3f(16, 10, 1), Vec3f(12, 2, 1), Vec3f(5, 16, 1)});

  renderTriangle(pts1, zBuffer, buffer, 1);
  Buffer<float> topFace(size, size, 0);
  Buffer<float> sideFace(size, size/2, 0);
  calcFormFactorPerCell(size, topFace, sideFace);

  float formFactors[] = {0.f, 0.f};
  calcFormFactorsFromBuffer(buffer, topFace, formFactors);

  float formFactor1 = formFactors[1];

  zBuffer.fillAll(0.f);
  buffer.fillAll(0);
  renderTriangle(pts2, zBuffer, buffer, 1);

  formFactors[0] = formFactors[1] = 0.f;
  calcFormFactorsFromBuffer(buffer, topFace, formFactors);

  REQUIRE(formFactor1 == formFactors[1]);
}

TEST_CASE("Render single hemicube face to colour buffer", "[hemicube]") {
  Model model("test/scene.obj", "test/scene.mtl");
  int faceIdx = 0;
  int gridSize = 200;
  Buffer<TGAColor> buffer(gridSize, gridSize, black);

  Face f = model.face(faceIdx);
  Vec3f up = (model.vert(f[1].ivert) - model.vert(f[0].ivert)).normalise();
  Vec3f dir = model.norm(faceIdx, 0);
  Vec3f eye = model.centreOf(faceIdx);

  Matrix MVP = formHemicubeMVP(eye, dir, up);
  renderModelReflectivity(buffer, model, MVP, eye, 0.05f);

  renderColourBuffer(buffer, "test/hemicube_face_colours.tga");
}

TEST_CASE("Render single hemicube face to ID buffer", "[hemicube]") {
  Model model("test/scene.obj", "test/scene.mtl");
  int nFaces = model.nfaces() + 1;
  int faceIdx = 0;
  int gridSize = 200;
  Buffer<int> buffer(gridSize, gridSize, 0);
  renderHemicubeFront(buffer, model, faceIdx);

  for(int j=0; j<buffer.height; ++j) {
    for(int i=0; i<buffer.width; ++i) {
      REQUIRE(buffer.get(i, j) < nFaces);
    }
  }

  renderIdsToColour(buffer, model, "test/hemicube_face_IDs.tga");
}

TEST_CASE("Render side hemicube face to ID buffer", "[hemicube]") {
  Model model("test/scene.obj", "test/scene.mtl");
  int nFaces = model.nfaces() + 1;
  int faceIdx = 0;
  int gridSize = 200;
  Buffer<int> buffer(gridSize, gridSize, 0);
  renderHemicubeUp(buffer, model, faceIdx);

  for(int j=0; j<buffer.height; ++j) {
    for(int i=0; i<buffer.width; ++i) {
      REQUIRE(buffer.get(i, j) < nFaces);
    }
  }

  renderIdsToColour(buffer, model, "test/hemicube_side_face_IDs.tga");
}

TEST_CASE("Render hemicube to ID index", "[hemicube]") {
  Model model("test/scene.obj", "test/scene.mtl");
  int nFaces = model.nfaces() + 1;
  int faceIdx = 8;
  int gridSize = 200;
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
  int gridSize = 200;
  std::vector<float> formFactors(model.nfaces()+1);

  calcFormFactorsFromModel(model, faceIdx, formFactors, gridSize);

  float sum = 0;
  for(int i=1; i<(int)formFactors.size(); ++i) {
    sum += formFactors[i];
  }
  // sum should be as close to 1 as possible
  REQUIRE(sum > 0.95f);
}

TEST_CASE("Render radiosity to texture", "[radiosity]") {
  Model model("test/red_green_walls.obj", "test/red_green_walls.mtl");
  int gridSize = 128;

  std::vector<Vec3f> radiosity(model.nfaces());
  calculateRadiosity(radiosity, model, gridSize, 1);

  std::vector<Vec3f> vertexRadiosity(model.nverts());
  radiosityFaceToVertex(vertexRadiosity, model, radiosity);

  renderVertexRadiosityToTexture(model, vertexRadiosity, 600, "test/radiosity_texture.tga");
}

//TEST_CASE("Render radiosity simple scene (single pass)", "[radiosity]") {
  //Model model("test/scene.obj", "test/scene.mtl");
  //int gridSize = 100;

  //std::vector<Vec3f> radiosity(model.nfaces());
  //calculateRadiosity(radiosity, model, gridSize, 5);

  //Vec3f eye(-4, -5, 6);
  //Vec3f dir = eye*-1;
  //Vec3f up(0, 0, 1);
  //int size = 800;
  //float nearPlane = 0.05f;
  //Matrix MVP = formHemicubeMVP(eye, dir, up);

  //Buffer<TGAColor> buffer(size, size, black);
  //renderModelRadiosity(buffer, model, MVP, eye, nearPlane, radiosity);

  //renderColourBuffer(buffer, "test/radiosity_single_pass.tga");
//}

//TEST_CASE("Render radiosity over complex scene", "[radiosity]") {
  //Model model("test/scene_subdivided.obj", "test/scene_subdivided.mtl");
  //int gridSize = 100;

  //std::vector<Vec3f> radiosity(model.nfaces());
  //calculateRadiosity(radiosity, model, gridSize, 5);

  //Vec3f eye(-4, -5, 6);
  //Vec3f dir = eye*-1;
  //Vec3f up(0, 0, 1);
  //int size = 800;
  //float nearPlane = 0.05f;
  //Matrix MVP = formHemicubeMVP(eye, dir, up);

  //Buffer<TGAColor> buffer(size, size, black);
  //renderModelRadiosity(buffer, model, MVP, eye, nearPlane, radiosity);

  //renderColourBuffer(buffer, "test/radiosity_complex.tga");
//}

//TEST_CASE("Render radiosity inside lit box", "[radiosity]") {
  //Model model("test/simple_box_subdivided.obj", "test/simple_box_subdivided.mtl");
  //int gridSize = 100;

  //std::vector<Vec3f> radiosity(model.nfaces());
  //calculateRadiosity(radiosity, model, 100, 5);

  //// Render
  //Vec3f eye(-1.5, 0, 0);
  //Vec3f dir(1, 0, 0);
  //Vec3f up(0, 0, 1);
  //int size = 800;
  //float nearPlane = 0.05f;
  //Matrix MVP = formHemicubeMVP(eye, dir, up);

  //Buffer<TGAColor> buffer(size, size, black);
  //renderModelRadiosity(buffer, model, MVP, eye, nearPlane, radiosity);

  //renderColourBuffer(buffer, "test/radiosity_box.tga");
//}
