#include "catch.hpp"
#include "geometry.hpp"

TEST_CASE("Ensure transform application works", "[vec3]") {
  Vec3f v(1,-1,3);
  Matrix m = Matrix::identity(4);
  int count = 1;
  for(int i=0; i<3; ++i) {
    for(int j=0; j<3; ++j) {
      m.set(i, j, count);
      count += 1;
    }
  }
  Vec3f result = applyTransform(m, v);
  REQUIRE(result == Vec3f(8,17,26));
}
