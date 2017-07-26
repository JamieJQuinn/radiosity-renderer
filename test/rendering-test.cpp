#include <iostream>

#include "catch.hpp"
#include "rendering.hpp"
#include "geometry.hpp"
#include "colours.hpp"

TEST_CASE("fillTriangle works on single triangle (const z)", "[renderer]") {
  int solution[] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,0,0,0,0,
0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,0,0,0,0,
0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,0,0,0,
0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,0,0,0,0,0,
0,0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  };

  int size = 20;
  Buffer<TGAColor> buffer(size, size, white);
  Buffer<float> zBuffer(size, size, 0);
  Vec3f pts[3] = {Vec3f(12, 2, 1), Vec3f(5, 16, 1), Vec3f(16, 10, 1)};
  renderTriangle(pts, zBuffer, buffer, red);

  for(int j=0; j<size; ++j) {
    for(int i=0; i<size; ++i) {
      REQUIRE(zBuffer.get(i, j) == solution[j*size+i]);
    }
  }

  renderZBuffer(zBuffer, "test/single_tri_const_z_zbuffer.tga");
  renderColourBuffer(buffer, "test/single_tri_const_z_colour.tga");
}

TEST_CASE("fillTriangle works on single triangle (varying z)", "[renderer]") {
  int solution[] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,1,2,3,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,2,2,3,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,2,2,3,4,5,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,2,3,4,4,5,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,2,3,4,4,5,6,7,0,0,0,0,
0,0,0,0,0,0,0,0,0,3,4,4,5,6,6,7,0,0,0,0,
0,0,0,0,0,0,0,0,3,4,4,5,6,6,7,8,9,0,0,0,
0,0,0,0,0,0,0,0,3,4,5,6,6,7,8,0,0,0,0,0,
0,0,0,0,0,0,0,3,4,5,6,6,7,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,4,5,5,6,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,4,5,5,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,5,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  };

  int size = 20;
  Buffer<TGAColor> buffer(size, size, white);
  Buffer<float> zBuffer(size, size, 0);
  Vec3f pts[3] = {Vec3f(12, 2, 1), Vec3f(5, 16, 5), Vec3f(16, 10, 9)};
  renderTriangle(pts, zBuffer, buffer, red);

  for(int j=0; j<size; ++j) {
    for(int i=0; i<size; ++i) {
      REQUIRE(int(zBuffer.get(i, j)) == solution[j*size+i]);
    }
  }

  renderZBuffer(zBuffer, "test/single_tri_vary_z_zbuffer.tga");
  renderColourBuffer(buffer, "test/single_tri_vary_z_colour.tga");
}

TEST_CASE("fillTriangle works on two triangles", "[renderer]") {
  int solution[] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,7,0,0,0,1,1,1,0,0,0,0,0,0,
0,0,0,0,0,0,0,7,0,0,0,1,1,1,0,0,0,0,0,0,
0,0,0,0,0,0,7,7,7,0,1,1,1,1,1,0,0,0,0,0,
0,0,0,0,0,0,7,7,7,0,1,1,1,1,1,0,0,0,0,0,
0,0,0,0,0,7,7,7,7,7,1,1,1,1,1,1,0,0,0,0,
0,0,0,0,0,7,7,7,7,7,7,1,1,1,1,1,0,0,0,0,
0,0,0,0,7,7,7,7,7,7,7,1,1,1,1,1,1,0,0,0,
0,0,0,0,0,0,7,7,7,7,7,7,1,1,1,0,0,0,0,0,
0,0,0,0,0,0,0,7,7,7,7,7,7,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,1,1,7,7,7,7,0,0,0,0,0,0,0,
0,0,0,0,0,0,1,1,1,0,7,7,7,7,0,0,0,0,0,0,
0,0,0,0,0,0,1,0,0,0,0,0,7,7,7,0,0,0,0,0,
0,0,0,0,0,1,0,0,0,0,0,0,0,7,7,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  };

  int size = 20;
  Buffer<TGAColor> buffer(size, size, white);
  Buffer<float> zBuffer(size, size, 0);
  Vec3f pts1[3] = {Vec3f(12, 2, 1), Vec3f(5, 16, 1), Vec3f(16, 10, 1)};
  Vec3f pts2[3] = {Vec3f(7, 4, 7.1), Vec3f(4, 10, 7.1), Vec3f(16, 18, 7.1)};
  renderTriangle(pts1, zBuffer, buffer, red);
  renderTriangle(pts2, zBuffer, buffer, blue);

  for(int j=0; j<size; ++j) {
    for(int i=0; i<size; ++i) {
      REQUIRE(int(zBuffer.get(i, j)) == solution[j*size+i]);
    }
  }

  renderZBuffer(zBuffer, "test/two_tri_const_z_zbuffer.tga");
  renderColourBuffer(buffer, "test/two_tri_const_z_colour.tga");
}

TEST_CASE("fillTriangle works on two intersecting triangles", "[renderer]") {
  int solution[] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,9,0,0,0,0,0,0,0,0,0,0,0,9,0,0,0,
0,0,0,0,9,8,0,0,0,0,0,0,0,0,0,8,9,0,0,0,
0,0,0,0,9,8,7,7,0,0,0,0,0,7,7,8,9,0,0,0,
0,0,0,0,9,8,7,7,6,6,0,6,6,7,7,8,9,0,0,0,
0,0,0,0,9,8,7,7,6,6,5,6,6,7,7,8,9,0,0,0,
0,0,0,0,9,8,7,7,6,6,5,6,6,7,7,8,9,0,0,0,
0,0,0,0,9,8,7,7,6,6,5,6,6,7,7,8,9,0,0,0,
0,0,0,0,9,8,7,7,6,6,5,6,6,7,7,8,9,0,0,0,
0,0,1,1,9,8,7,7,6,6,5,6,6,7,7,8,9,1,1,0,
0,0,0,0,9,8,7,7,6,6,5,6,6,7,7,8,9,0,0,0,
0,0,0,0,9,8,7,7,6,6,5,6,6,7,7,8,9,0,0,0,
0,0,0,0,9,8,7,7,6,6,5,6,6,7,7,8,9,0,0,0,
0,0,0,0,9,8,7,7,6,6,5,6,6,7,7,8,9,0,0,0,
0,0,0,0,9,8,7,7,6,6,0,6,6,7,7,8,9,0,0,0,
0,0,0,0,9,8,7,7,0,0,0,0,0,7,7,8,9,0,0,0,
0,0,0,0,9,8,0,0,0,0,0,0,0,0,0,8,9,0,0,0,
0,0,0,0,9,0,0,0,0,0,0,0,0,0,0,0,9,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  };

  int size = 20;
  Buffer<TGAColor> buffer(size, size, white);
  Buffer<float> zBuffer(size, size, 0);
  Vec3f pts1[3] = {Vec3f(4, 2, 9), Vec3f(4, 18, 9), Vec3f(18, 10, 1)};
  Vec3f pts2[3] = {Vec3f(16, 2, 9), Vec3f(2, 10, 1), Vec3f(16, 18, 9)};
  renderTriangle(pts1, zBuffer, buffer, red);
  renderTriangle(pts2, zBuffer, buffer, blue);

  for(int j=0; j<size; ++j) {
    for(int i=0; i<size; ++i) {
      REQUIRE(int(zBuffer.get(i, j)) == solution[j*size+i]);
    }
  }

  renderZBuffer(zBuffer, "test/two_tri_intersect_zbuffer.tga");
  renderColourBuffer(buffer, "test/two_tri_intersect_colour.tga");
}

TEST_CASE("Test calculation of form factors", "[form_factors]") {
  int size = 50;

  Model model("test/simple_box.obj", "test/simple_box.mtl");

  Vec3f eye(-2, -2.5, 3);
  Vec3f centre(0,0,0);
  Vec3f up(0, 1, 0);
  Matrix modelView = lookAt(eye, centre, up);

  Matrix P = Matrix::identity(4);
  P.set(3, 2, -1.f/(eye-centre).norm());
  Matrix V = viewportRelative(size/4, size/4, size/2, size/2, 255);
  Matrix MVP = V*P*modelView;

  Buffer<int> itemBuffer(size, size, 0);
  std::vector<float> formFactors(model.nfaces()+1, 0.f);

  renderModel(itemBuffer, model, MVP);

  Buffer<float> topFace(size, size);
  Buffer<float> sideFace(size, size/2);
  calcFormFactorPerCell(size, topFace, sideFace);

  calcFormFactors(itemBuffer, topFace, formFactors);
}

TEST_CASE("Test calculation of form factors per cell", "[form_factors]") {
  int size = 200;
  Buffer<float> topFace(size, size);
  Buffer<float> sideFace(size, size/2);
  calcFormFactorPerCell(size, topFace, sideFace);

  REQUIRE(topFace.sum() + 4*sideFace.sum() == Approx(1.0f));
}
