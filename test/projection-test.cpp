#include "catch.hpp"
#include "buffer.hpp"
#include "geometry.hpp"
#include "model.hpp"
#include "tgaimage.hpp"
#include "rendering.hpp"
#include "colours.hpp"

TEST_CASE("Test orthographic projection", "[projection]") {
  int size = 800;

  Model model("test/simple_box.obj");

  Matrix MVP = viewportRelative(size/4, size/4, size/2, size/2, 255);
  Buffer<TGAColor> buffer(size, size, black);

  renderWireFrame(model, buffer, MVP);

  renderColourBuffer(buffer, "test/orthographic_projection.tga");
}

TEST_CASE("Test perspective projection", "[projection]") {
  int size = 800;

  Model model("test/simple_box.obj");

  Vec3f camera(0,0,3);
  Matrix P = Matrix::identity(4);
  P.set(3, 2, -1.f/camera.z);
  Matrix V = viewportRelative(size/4, size/4, size/2, size/2, 255);
  Matrix MVP = V*P;
  Buffer<TGAColor> buffer(size, size, black);

  renderWireFrame(model, buffer, MVP);

  renderColourBuffer(buffer, "test/perspective_projection.tga");
}

TEST_CASE("Test model view", "[camera]") {
  int size = 800;

  Model model("test/simple_box.obj");

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

TEST_CASE("Test model view (filled triangles)", "[camera]") {
  int size = 800;

  Model model("test/simple_box.obj");

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

  for (int i=0; i<model.nfaces(); ++i) {
    std::vector<int> face = model.face(i);
    Vec3f screen_coords[3];
    Vec3f world_coords[3];
    for (int j=0; j<3; j++) {
      Vec3f v = model.vert(face[j]);
      screen_coords[j] = m2v(MVP*v2m(v));
      world_coords[j] = v;
    }
    Vec3f n = (world_coords[2]-world_coords[0]).cross(world_coords[1]-world_coords[0]);
    n.normalise();
    float intensity = n.dot(eye-centre);
    if (intensity<0) {
      renderTriangle(screen_coords, zBuffer, buffer, red);
    }
  }

  renderColourBuffer(buffer, "test/model_view_filled.tga");
}

TEST_CASE("Test viewing subdivided model", "[model]") {
  int size = 800;

  Model model("test/simple_box_subdivided.obj");

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

  renderColourBuffer(buffer, "test/simple_box_subdivided.tga");
}

//TEST_CASE("Test viewing subdivided model from inside", "[model]") {
  //const TGAColor black = TGAColor(0, 0, 0, 255);
  //int size = 800;

  //Model model("test/simple_box_subdivided.obj");

  //Vec3f eye(0.5f, 0.0f, 0.0f);
  //Vec3f centre(0,0,0);
  //Vec3f up(0, 1, 0);
  //Matrix modelView = lookAt(eye, centre, up);

  //Matrix P = Matrix::identity(4);
  //P.set(3, 2, -1.f/(eye-centre).norm());
  //Matrix V = viewportRelative(size/4, size/4, size/2, size/2, 255);
  //Matrix MVP = V*P*modelView;

  //std::cout << MVP << std::endl;

  //Buffer<TGAColor> buffer(size, size, black);
  //renderWireFrame(model, buffer, MVP);

  //renderColourBuffer(buffer, "test/simple_box_subdivided_inside.tga");
//}

TEST_CASE("Test viewing subdivided model from inside (filled triangles)", "[camera]") {
  int size = 800;

  Model model("test/simple_box_subdivided.obj");

  Vec3f eye(0.5f, 0.0f, 0.0f);
  Vec3f centre(-0.5,0,0);
  Vec3f up(0, 1, 0);
  Matrix modelView = lookAt(eye, centre, up);

  Matrix P = Matrix::identity(4);
  P.set(3, 2, -1.f/(eye-centre).norm());
  Matrix V = viewportRelative(size/4, size/4, size/2, size/2, 255);
  Matrix MVP = V*P*modelView;

  Buffer<TGAColor> buffer(size, size, black);
  Buffer<float> zBuffer(size, size, 0);

  for (int i=0; i<model.nfaces(); ++i) {
    std::vector<int> face = model.face(i);
    Vec3f screen_coords[4];
    Vec3f world_coords[4];
    for (int j=0; j<4; j++) {
      Vec3f v = model.vert(face[j]);
      screen_coords[j] = m2v(MVP*v2m(v));
      world_coords[j] = v;
    }
    Vec3f n = (world_coords[2]-world_coords[0]).cross(world_coords[1]-world_coords[0]);
    n.normalise();
    float intensity = n.dot(eye-centre);
    int bitmask = 0;
    for(int i=0; i<4; ++i) {
      bitmask += (screen_coords[0].z < 255);
      bitmask += (screen_coords[0].z > 0);
    }
    if(bitmask == 8){
      renderTriangle(screen_coords, zBuffer, buffer, red);
      renderTriangle(screen_coords+1, zBuffer, buffer, yellow);
    }
  }

  renderZBuffer(zBuffer, "test/simple_box_subdivided_inside_filled_zbuffer.tga");
  renderColourBuffer(buffer, "test/simple_box_subdivided_inside_filled.tga");
}
