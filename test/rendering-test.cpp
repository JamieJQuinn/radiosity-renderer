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
  std::vector<Vec3f> pts({Vec3f(12, 2, 1), Vec3f(5, 16, 1), Vec3f(16, 10, 1)});
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
  std::vector<Vec3f> pts({Vec3f(12, 2, 1), Vec3f(5, 16, 5), Vec3f(16, 10, 9)});
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
  std::vector<Vec3f> pts1({Vec3f(12, 2, 1), Vec3f(5, 16, 1), Vec3f(16, 10, 1)});
  std::vector<Vec3f> pts2({Vec3f(7, 4, 7.1), Vec3f(4, 10, 7.1), Vec3f(16, 18, 7.1)});
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
  std::vector<Vec3f> pts1({Vec3f(4, 2, 9), Vec3f(4, 18, 9), Vec3f(18, 10, 1)});
  std::vector<Vec3f> pts2({Vec3f(16, 2, 9), Vec3f(2, 10, 1), Vec3f(16, 18, 9)});
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

TEST_CASE("Test calculation of form factors per cell", "[form_factors]") {
  int size = 200;
  Buffer<float> topFace(size, size);
  Buffer<float> sideFace(size, size/2);
  calcFormFactorPerCell(size, topFace, sideFace);

  REQUIRE(topFace.sum() + 4*sideFace.sum() == Approx(1.0f));
}

TEST_CASE("Test clipping against back wall (no need to clip)", "[clipping]") {
  Buffer<TGAColor> buffer(500, 500, black);
  Buffer<float> zBuffer(buffer.width, buffer.height, 0.f);
  TGAColor colour(255, 0, 0, 255);

  std::vector<Vec4f> pts(3);
  pts[0] = Vec3f(-0.5, -0.5, -0.5f);
  pts[1] = Vec3f(0, 0.8, -0.6f);
  pts[2] = Vec3f(0.7, -0.4, -0.4f);

  clipAndRenderTriangle(pts, zBuffer, buffer, colour);

  renderColourBuffer(buffer, "test/clipping_test_no_need.tga");
}

TEST_CASE("Test clipping against back wall splitting tri", "[clipping]") {
  Buffer<TGAColor> buffer(500, 500, black);
  Buffer<float> zBuffer(buffer.width, buffer.height, 0.f);
  TGAColor colour(255, 0, 0, 255);

  std::vector<Vec4f> pts(3);
  pts[0] = Vec3f(0, 0.8, -0.6f);
  pts[1] = Vec3f(-0.5, -0.5, -0.5f);
  pts[2] = Vec3f(0.7, -0.4, 1.4f);

  clipAndRenderTriangle(pts, zBuffer, buffer, colour);

  renderColourBuffer(buffer, "test/clipping_test_split.tga");
}

TEST_CASE("Test clipping against back wall without splitting tri", "[clipping]") {
  Buffer<TGAColor> buffer(500, 500, black);
  Buffer<float> zBuffer(buffer.width, buffer.height, 0.f);
  TGAColor colour(255, 0, 0, 255);

  std::vector<Vec4f> pts(3);
  pts[0] = Vec3f(-0.5, -0.5, -0.5f);
  pts[1] = Vec3f(0, 0.8, 1.6f);
  pts[2] = Vec3f(0.7, -0.4, 1.4f);

  clipAndRenderTriangle(pts, zBuffer, buffer, colour);

  renderColourBuffer(buffer, "test/clipping_test_nosplit.tga");
}

TEST_CASE("Test clipping in extreme situation", "[clipping]") {
  Buffer<TGAColor> buffer(500, 500, black);
  Buffer<float> zBuffer(buffer.width, buffer.height, 0.f);
  TGAColor colour(255, 0, 0, 255);

  std::vector<Vec4f> pts(3);
  pts[0] = Vec3f(25250, -49750.1, 5.01003);
  pts[1] = Vec3f(25250, 50250.1, 5.01003);
  pts[2] = Vec3f(-0.5, -0.5, -0.1);

  clipAndRenderTriangle(pts, zBuffer, buffer, colour);

  renderColourBuffer(buffer, "test/clipping_test_extreme.tga");
}
