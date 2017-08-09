#include "catch.hpp"
#include "buffer.hpp"
#include "geometry.hpp"
#include "model.hpp"
#include "tgaimage.hpp"
#include "rendering.hpp"
#include "colours.hpp"
#include "hemicube.hpp"

TEST_CASE("Test perspective projection", "[projection]") {
  int size = 800;

  Model model("test/simple_box.obj", "test/simple_box.mtl");

  Vec3f camera(0,0,3);
  Matrix P = Matrix::identity(4);
  P.set(3, 2, -1.f/camera.z);
  Matrix V = viewportRelative(size/4, size/4, size/2, size/2);
  Matrix MVP = V*P;
  Buffer<TGAColor> buffer(size, size, black);

  renderWireFrame(model, buffer, MVP);

  renderColourBuffer(buffer, "test/perspective_projection.tga");
}

TEST_CASE("Test moving the camera (perspective)", "[camera]") {
  Model model("test/simple_box.obj", "test/simple_box.mtl");

  Vec3f eye(2, 2.5, 3);
  Vec3f dir = eye*-1;
  Vec3f up(0, 0, 1);
  int size = 800;
  Matrix MVP = formHemicubeMVP(eye, dir, up);

  Buffer<TGAColor> buffer(size, size, black);
  renderWireFrame(model, buffer, MVP);

  renderColourBuffer(buffer, "test/moved_camera.tga");
}

TEST_CASE("Ensure MVP retains z order", "[projection]") {
  Vec3f eye(0, 0, 3);
  Vec3f dir = (eye*-1).normalise();
  Vec3f up(0, 1, 0);
  int size = 800;

  float nearPlane = .05f;
  float farPlane = 100.f;

  Vec4f v(1,1,1,1);
  Vec4f w(-1,-1,-1,1);

  Matrix translation = formTranslation(eye*-1);
  Matrix view = lookAt(Vec3f(0, 0, 0), dir, up)*translation;
  Vec4f vAfterView = view*v;
  Vec4f wAfterView = view*w;
  REQUIRE(vAfterView.z > wAfterView.z);

  Matrix projection = formRightAngledProjection(nearPlane, farPlane);
  Vec4f vAfterProjection = projection*vAfterView;
  Vec4f wAfterProjection = projection*wAfterView;
  REQUIRE(vAfterProjection.z > wAfterProjection.z);

  REQUIRE(vAfterProjection.z > -farPlane);
  REQUIRE(vAfterProjection.z < nearPlane);
  REQUIRE(wAfterProjection.z > -farPlane);
  REQUIRE(wAfterProjection.z < nearPlane);

  vAfterProjection.homogenise();
  wAfterProjection.homogenise();

  REQUIRE(vAfterProjection.z > -1.0f);
  REQUIRE(vAfterProjection.z < 1.0f);
  REQUIRE(wAfterProjection.z > -1.0f);
  REQUIRE(wAfterProjection.z < 1.0f);

  Matrix viewport = viewportRelative(0, 0, size, size);
  Vec4f vAfterViewport = viewport*vAfterProjection;
  Vec4f wAfterViewport = viewport*wAfterProjection;
  REQUIRE(vAfterViewport.z > wAfterViewport.z);

  REQUIRE(vAfterViewport.z > 0.f);
  REQUIRE(vAfterViewport.z < 1.f);
  REQUIRE(wAfterViewport.z > 0.f);
  REQUIRE(wAfterViewport.z < 1.f);
}

TEST_CASE("Test moving the camera (filled triangles)", "[camera]") {
  Model model("test/simple_box.obj", "test/simple_box.mtl");

  Vec3f eye(2, 2.5, 3);
  Vec3f dir = eye*-1;
  Vec3f up(0, 0, 1);
  int size = 800;
  Matrix MVP = formHemicubeMVP(eye, dir, up);

  Buffer<TGAColor> buffer(size, size, black);

  renderTestModelReflectivity(buffer, model, MVP, dir, 0.05f);

  renderColourBuffer(buffer, "test/simple_box_filled.tga");
}

TEST_CASE("Test viewing subdivided model from inside (filled triangles)", "[camera]") {
  Model model("test/simple_box_subdivided.obj", "test/simple_box_subdivided.mtl");

  Vec3f dir = Vec3f(0,1,0);
  Vec3f eye(0.2f, -.5f, 0.3f);
  Vec3f up(0, 0, 1);
  int size = 800;
  Matrix MVP = formHemicubeMVP(eye, dir, up);

  Buffer<TGAColor> buffer(size, size, black);

  renderTestModelReflectivity(buffer, model, MVP, dir, 0.05f);

  renderColourBuffer(buffer, "test/simple_box_subdivided_inside.tga");
}

TEST_CASE("Test projection of vertices close to near plane", "[projection]") {
  Model model("test/scene.obj", "test/scene.mtl");

  int gridSize = 500;

  Vec3f up(0,0,1);
  Vec3f dir(0,1,0);
  Vec3f eye = Vec3f(1,-2,2);
  Matrix MVP = formHemicubeMVP(eye, dir, up);
  //Matrix translation = formTranslation(eye*-1);
  //Matrix view = lookAt(Vec3f(0, 0, 0), dir, up)*translation;
  //Matrix projection = formRightAngledProjection(0.05f, 20.0f);
  //Matrix MVP = projection*view*translation;

  Buffer<TGAColor> buffer(gridSize, gridSize, black);
  Buffer<float> zBuffer(buffer.width, buffer.height, -1.f);
  TGAColor colour(255, 0, 0, 255);

  // Render red wall

  Vec4f tri1[3];
  tri1[0] = Vec3f(2,-2,0);
  tri1[1] = Vec3f(2,2,4);
  tri1[2] = Vec3f(2,2,0);

  Vec4f tri2[3];
  tri2[0] = Vec3f(2,-2,0);
  tri2[1] = Vec3f(2,-2,4);
  tri2[2] = Vec3f(2,2,4);

  std::vector<Vec4f> screen_coords(3);
  for (int j=0; j<3; j++) {
    Vec4f v = tri1[j];
    screen_coords[j] = MVP*v;
  }
  clipAndRenderTriangle(screen_coords, zBuffer, buffer, colour, 0.05f);

  for (int j=0; j<3; j++) {
    Vec4f v = tri2[j];
    screen_coords[j] = MVP*v;
  }
  clipAndRenderTriangle(screen_coords, zBuffer, buffer, colour, 0.05f);

  // Render blue wall

  colour.r = 0;
  colour.b = 255;

  tri1[0] = Vec3f(2,-2,4);
  tri1[1] = Vec3f(-2,-2,4);
  tri1[2] = Vec3f(-2,2,4);

  tri2[0] = Vec3f(2,-2,4);
  tri2[1] = Vec3f(-2,2,4);
  tri2[2] = Vec3f(2,2,4);

  for (int j=0; j<3; j++) {
    Vec4f v = tri1[j];
    screen_coords[j] = MVP*v;
  }
  clipAndRenderTriangle(screen_coords, zBuffer, buffer, blue, 0.05f);

  for (int j=0; j<3; j++) {
    Vec4f v = tri2[j];
    screen_coords[j] = MVP*v;
  }
  clipAndRenderTriangle(screen_coords, zBuffer, buffer, blue, 0.05f);

  renderColourBuffer(buffer, "test/close_vertices_red_blue_walls.tga");

  buffer.fillAll(black);
  renderTestModelReflectivity(buffer, model, MVP, dir, 0.05f);

  renderColourBuffer(buffer, "test/close_vertices.tga");
}
