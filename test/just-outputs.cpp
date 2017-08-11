#include "catch.hpp"
#include "buffer.hpp"
#include "geometry.hpp"
#include "model.hpp"
#include "tgaimage.hpp"
#include "rendering.hpp"
#include "colours.hpp"
#include "hemicube.hpp"

TEST_CASE("Output scene + z-buffer", "[output]") {
  Model model("test/scene.obj", "test/scene.mtl");

  int gridSize = 500;
  float nearPlane = 0.05f;

  // Choose random face
  int faceIdx = 0;

  Face f = model.face(faceIdx);
  //Vec3f up = (model.vert(f[1].ivert) - model.vert(f[0].ivert)).normalise();
  //Vec3f dir = model.norm(faceIdx, 0);
  Vec3f up(0,0,1);
  Vec3f dir(1,0,0);
  //Vec3f eye = model.centreOf(faceIdx);
  Vec3f eye(-2,1,2);

  Matrix MVP = formHemicubeMVP(eye, dir, up);

  Buffer<TGAColor> buffer(gridSize, gridSize, black);
  TGAColor colours[] = {red, green, blue, yellow, white};
  int colourIdx = 0;

  Buffer<float> zBuffer(buffer.width, buffer.height, 0.f);
  for (int i=0; i<model.nfaces(); ++i) {
    Face face = model.face(i);
    TGAColor colour = model.getFaceColour(face);

    std::vector<Vec4f> pts = transformFace(face, model, MVP);

    Vec3f n = model.norm(i, 0);
    if( n.dot(model.centreOf(i)-eye) <= 0.f ) {
      clipAndRenderTriangle(pts, zBuffer, buffer, colour, nearPlane);
      ++colourIdx;
      colourIdx %= 5;
    }
  }

  renderColourBuffer(buffer, "test/scene_inside.tga");
  renderZBuffer(zBuffer, "test/scene_inside_z.tga");
}

TEST_CASE("Output z-buffer of single triangle", "[output]") {
  int size = 500;
  Buffer<TGAColor> buffer(size, size, black);
  Buffer<float> zBuffer(size, size, 0);
  std::vector<Vec3f> pts1({Vec3f(0.6, 0.1, 0.05)*size, Vec3f(0.25, 0.8, 0.25)*size, Vec3f(0.8, 0.5, 0.5)*size});
  std::vector<Vec3f> pts2({Vec3f(0.35, 0.2, 0.25)*size, Vec3f(0.2, 0.5, 0.35)*size, Vec3f(0.8, 0.9, 0.35)*size});
  renderTriangle(pts1, zBuffer, buffer, red);
  renderTriangle(pts2, zBuffer, buffer, blue);

  renderZBuffer(zBuffer, "test/two_tri_zbuffer.tga");
  renderColourBuffer(buffer, "test/two_tri_colour.tga");
}
