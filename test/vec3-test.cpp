#include "catch.hpp"
#include "geometry.hpp"

TEST_CASE("Ensure length works", "[vec3]") {
  REQUIRE(Vec3f(1, 2, 3).norm2() == Approx(14));
  REQUIRE(Vec3f(1, 0, 0).norm() == Approx(1));
}

TEST_CASE("Ensure scalar multiplication works", "[vec3]") {
  Vec3f v(1, 1, 1);
  Vec3f v2(v*2);
  REQUIRE(v2 == Vec3f(2, 2, 2));
}

TEST_CASE("Ensure boolean ops work", "[vec3]") {
  Vec3f v1(1, 1, 1);
  Vec3f v2(1, 1, 1);
  Vec3f v3(1, 0, 0);
  REQUIRE(v1 == v2);
  REQUIRE(v1 != v3);
}

TEST_CASE("Ensure normalisation works", "[vec3]") {
  Vec3f v(2, 3, 5);
  v.normalise();
  REQUIRE(v.norm() == Approx(1.0f));
}
