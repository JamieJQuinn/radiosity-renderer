#include "catch.hpp"
#include "geometry.hpp"

TEST_CASE("Ensure matrix multiplacation works", "[matrix]") {
  Vec4f v(1,-1,3,1);
  Matrix m = Matrix::identity(4);
  int count = 1;
  for(int i=0; i<3; ++i) {
    for(int j=0; j<3; ++j) {
      m.set(i, j, count);
      count += 1;
    }
  }
  Vec4f result = m*v;
  REQUIRE(result == Vec4f(8,17,26,1));
}

TEST_CASE("Ensure matrix multiplacation works 2", "[matrix]") {
  Vec4f v(1,-1,2,3);
  float mVals[16] = {
    1, 2, 3, 4,
    4, 3, 2, 1,
    1, 3, 1, 4,
    6, 5, 4, 3
  };
  Matrix m = Matrix::identity(4);
  for(int i=0; i<4; ++i) {
    for(int j=0; j<4; ++j) {
      m.set(i, j, mVals[i*4+j]);
    }
  }
  Vec4f result = m*v;
  REQUIRE(result == Vec4f(17, 8, 12, 18));
}
