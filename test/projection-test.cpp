#include "catch.hpp"
#include "buffer.hpp"
#include "geometry.hpp"
#include "model.hpp"
#include "tgaimage.hpp"
#include "rendering.hpp"
#include "colours.hpp"

TEST_CASE("Test perspective projection", "[projection]") {
  int size = 800;

  Model model("test/simple_box.obj", "test/simple_box.mtl");

  Vec3f camera(0,0,3);
  Matrix P = Matrix::identity(4);
  P.set(3, 2, -1.f/camera.z);
  Matrix V = viewportRelative(size/4, size/4, size/2, size/2, 255);
  Matrix MVP = V*P;
  Buffer<TGAColor> buffer(size, size, black);

  renderWireFrame(model, buffer, MVP);

  renderColourBuffer(buffer, "test/perspective_projection.tga");
}

TEST_CASE("Test moving the camera (perspective)", "[camera]") {
  int size = 800;

  Model model("test/simple_box.obj", "test/simple_box.mtl");

  Vec3f eye(2, 2.5, 3);
  Vec3f centre(0,0,0);
  Vec3f up(0, 1, 0);
  Matrix modelView = lookAt(eye, centre, up);

  Matrix P = Matrix::identity(4);
  P.set(3, 2, -1.f/(eye-centre).norm());
  Matrix V = viewportRelative(size/4, size/4, size/2, size/2, 255);
  Matrix MVP = V*P*modelView;

  Buffer<TGAColor> buffer(size, size, black);
  renderWireFrame(model, buffer, MVP);

  renderColourBuffer(buffer, "test/model_view.tga");
}

TEST_CASE("Test moving the camera (filled triangles)", "[camera]") {
  int size = 800;

  Model model("test/simple_box.obj", "test/simple_box.mtl");

  Vec3f eye(2, 2.5, 3);
  Vec3f centre(0,0,0);
  Vec3f up(0, 1, 0);
  Matrix modelView = lookAt(eye, centre, up);

  Matrix P = Matrix::identity(4);
  P.set(3, 2, -1.f/(eye-centre).norm());
  Matrix V = viewportRelative(size/4, size/4, size/2, size/2, 255);
  Matrix MVP = V*P*modelView;

  Buffer<TGAColor> buffer(size, size, black);
  Buffer<float> zBuffer(size, size, -255);

  renderTestModel(buffer, model, MVP);

  //renderZBuffer(zBuffer, "test/model_view_filled_zbuffer.tga");
  renderColourBuffer(buffer, "test/model_view_filled.tga");
}

TEST_CASE("Test viewing subdivided model from inside (filled triangles)", "[camera]") {
  int size = 800;

  Model model("test/simple_box_subdivided.obj", "test/simple_box_subdivided.mtl");

  Vec3f eye(1.0f, 0.5f, 0.25f);
  Vec3f dir(0, 1.0f, 0);
  Vec3f centre(eye-dir);
  Vec3f up(-1.f, 0, 0);
  Matrix translation = Matrix::identity(4);
  for(int i=0; i<3; ++i) {
    translation.set(i, 3, -eye[i]);
  }
  Matrix view = lookAt(eye, centre, up)*translation;

  Matrix projection = Matrix::identity(4);
  projection.set(3, 2, -1.f/(eye-centre).norm());
  Matrix viewport = viewportRelative(size/4, size/4, size/2, size/2, 255);
  Matrix MVP = viewport*projection*view;

  Buffer<TGAColor> buffer(size, size, black);

  renderTestModel(buffer, model, MVP);

  renderColourBuffer(buffer, "test/simple_box_subdivided_inside_filled.tga");
}
