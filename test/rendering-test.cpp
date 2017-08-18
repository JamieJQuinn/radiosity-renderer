#include <iostream>

#include "catch.hpp"
#include "rendering.hpp"
#include "geometry.hpp"
#include "colours.hpp"
#include "hemicube.hpp"

TEST_CASE("rasterising works for single triangle", "[renderer]") {
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
}

TEST_CASE("rasterising works on a single triangle with varying z", "[renderer]") {
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
}

TEST_CASE("rasteriser & z-buffer works with non-intersecting tris", "[renderer]") {
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
}

TEST_CASE("Test areas of two triangles making up a quad are identicle", "[rendering]") {
  int size = 512;
  Buffer<int> buffer(size, size, 0);
  Buffer<float> zBuffer(size, size, 0);
  std::vector<Vec3f> pts1({Vec3f(0.1, 0.1, 1)*size, Vec3f(0.9, 0.1, 1)*size, Vec3f(0.9, 0.9, 1)*size});
  std::vector<Vec3f> pts2({Vec3f(0.9, 0.9, 7.1)*size, Vec3f(0.1, 0.9, 7.1)*size, Vec3f(0.1, 0.1, 7.1)*size});
  renderTriangle(pts1, zBuffer, buffer, 1);
  renderTriangle(pts2, zBuffer, buffer, 2);

  Buffer<float> topFace(size, size, 0);
  Buffer<float> sideFace(size, size/2, 0);
  calcFormFactorPerCell(size, topFace, sideFace);

  float formFactors[] = {0.f, 0.f, 0.f};
  calcFormFactorsFromBuffer(buffer, topFace, formFactors);

  REQUIRE(formFactors[1] == Approx(formFactors[2]).epsilon(0.001));
}

TEST_CASE("rasteriser & z-buffer works with intersecting tris", "[renderer]") {
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
}

TEST_CASE("Test calculation of form factors per cell", "[form_factors]") {
  int size = 512;
  Buffer<float> topFace(size, size);
  Buffer<float> sideFace(size, size/2);
  calcFormFactorPerCell(size, topFace, sideFace);

  REQUIRE(topFace.sum() + 4*sideFace.sum() == Approx(1.0f));

  renderZBuffer(topFace, "test/topFaceFactors.tga");
  renderZBuffer(sideFace, "test/sideFaceFactors.tga");
}

TEST_CASE("Test clipping produces correct w values", "[clipping]") {
  std::vector<Vec4f> pts(3);
  pts[0] = Vec3f(-0.5, -0.5, -0.5f);
  pts[1] = Vec3f(0, 0.8, 1.6f);
  pts[2] = Vec3f(0.7, -0.4, 1.4f);

  float nearPlane = 0.05f;

  clipTriangle(pts, nearPlane);

  REQUIRE(pts[1].w == nearPlane);
  REQUIRE(pts[2].w == nearPlane);
}

TEST_CASE("Test clipping against back wall (no need to clip)", "[clipping]") {
  Buffer<TGAColor> buffer(500, 500, black);
  Buffer<float> zBuffer(buffer.width, buffer.height, 0.f);

  std::vector<Vec4f> pts(3);
  pts[0] = Vec3f(-0.5, -0.5, -0.5f);
  pts[1] = Vec3f(0, 0.8, -0.6f);
  pts[2] = Vec3f(0.7, -0.4, -0.4f);

  float nearPlane = 0.05f;
  int numTriangles = clipTriangle(pts, nearPlane);
  REQUIRE(numTriangles == 1);
  Matrix viewport = viewportRelative(0, 0, buffer.width, buffer.height);
  for(int i=0; i<numTriangles*3; i+=3) {
    // Transform into viewport
    for(int j=0; j<3; ++j) {
      pts[j+i] = viewport*pts[j+i];
    }
    renderTriangle(pts[i+0], pts[i+1], pts[i+2], zBuffer, buffer, red);
  }

  renderColourBuffer(buffer, "test/clipping_test_no_need.tga");
}

TEST_CASE("Test clipping against back wall (clipping away entire tri)", "[clipping]") {
  std::vector<Vec4f> pts(3);
  pts[0] = Vec3f(-0.5, -0.5, 0.5f);
  pts[1] = Vec3f(0, 0.8, 0.6f);
  pts[2] = Vec3f(0.7, -0.4, 0.4f);

  float nearPlane = 0.05f;
  int numTriangles = clipTriangle(pts, nearPlane);
  REQUIRE(numTriangles == 0);
}

TEST_CASE("Test clipping against back wall splitting tri", "[clipping]") {
  Buffer<TGAColor> buffer(500, 500, black);
  Buffer<float> zBuffer(buffer.width, buffer.height, 0.f);

  std::vector<Vec4f> pts(3);
  pts[0] = Vec3f(0, 0.8, -0.6f);
  pts[1] = Vec3f(-0.5, -0.5, -0.5f);
  pts[2] = Vec3f(0.7, -0.4, 1.4f);

  float nearPlane = 0.05f;
  int numTriangles = clipTriangle(pts, nearPlane);
  // Simulate homogenisation without actually doing it, that messes with x-y coords
  for(int i=0; i<(int)pts.size(); ++i) {
    pts[i].w = 1;
  }
  REQUIRE(numTriangles == 2);
  Matrix viewport = viewportRelative(0, 0, buffer.width, buffer.height);
  for(int i=0; i<numTriangles*3; i+=3) {
    // Transform into viewport
    for(int j=0; j<3; ++j) {
      pts[j+i] = viewport*pts[j+i];
    }
    renderTriangle(pts[i+0], pts[i+1], pts[i+2], zBuffer, buffer, red);
  }

  renderColourBuffer(buffer, "test/clipping_test_split.tga");
}

TEST_CASE("Test clipping against back wall without splitting tri", "[clipping]") {
  Buffer<TGAColor> buffer(500, 500, black);
  Buffer<float> zBuffer(buffer.width, buffer.height, 0.f);

  std::vector<Vec4f> pts(3);
  pts[0] = Vec3f(0, 0.8, 0.5f);
  pts[1] = Vec3f(-0.5, -0.5, -0.5f);
  pts[2] = Vec3f(0.7, -0.4, 1.4f);

  float nearPlane = 0.05f;
  int numTriangles = clipTriangle(pts, nearPlane);
  REQUIRE(numTriangles == 1);
  for(int i=0; i<(int)pts.size(); ++i) {
    pts[i].w = 1;
  }
  Matrix viewport = viewportRelative(0, 0, buffer.width, buffer.height);
  for(int i=0; i<numTriangles*3; i+=3) {
    // Transform into viewport
    for(int j=0; j<3; ++j) {
      pts[j+i] = viewport*pts[j+i];
    }
    renderTriangle(pts[i+0], pts[i+1], pts[i+2], zBuffer, buffer, red);
  }

  renderColourBuffer(buffer, "test/clipping_test_nosplit.tga");
}
