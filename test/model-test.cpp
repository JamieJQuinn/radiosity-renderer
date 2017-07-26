#include "catch.hpp"
#include "model.hpp"
#include "tgaimage.hpp"
#include "geometry.hpp"
#include "rendering.hpp"
#include "colours.hpp"

TEST_CASE("Test loading of materials", "[model]") {
  int size = 800;

  Model model("test/scene.obj", "test/scene.mtl");

  Vec3f eye(-1.f, 2.0f, 1.0f);
  Vec3f dir(-1.0f, 0, 0);
  Vec3f centre(eye-dir);
  Vec3f up(0, 1, 0);
  Matrix translation = Matrix::identity(4);
  for(int i=0; i<3; ++i) {
    translation.set(i, 3, -eye[i]);
  }
  Matrix view = lookAt(eye, centre, up)*translation;

  Matrix projection = Matrix::identity(4);
  projection.set(3, 2, -1.f);
  Matrix viewport = viewportRelative(0, 0, size, size, 255);
  Matrix MVP = viewport*projection*view;

  Buffer<TGAColor> buffer(size, size, black);

  renderTestModelReflectivity(buffer, model, MVP);

  renderColourBuffer(buffer, "test/scene_inside.tga");
}

TEST_CASE("Test viewing subdivided model", "[model]") {
  int size = 800;

  Model model("test/simple_box_subdivided.obj", "test/simple_box_subdivided.mtl");

  Vec3f eye(-2, -2.5, -3);
  Vec3f centre(0,0,0);
  Vec3f up(0, 1, 0);
  Matrix modelView = lookAt(eye, centre, up);

  Matrix P = Matrix::identity(4);
  P.set(3, 2, -1.f/(eye-centre).norm());
  Matrix V = viewportRelative(size/4, size/4, size/2, size/2, 255);
  Matrix MVP = V*P*modelView;

  Buffer<TGAColor> buffer(size, size, black);
  renderTestModel(buffer, model, MVP);

  renderColourBuffer(buffer, "test/simple_box_subdivided_filled.tga");
}
