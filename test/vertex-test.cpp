#include "catch.hpp"
#include "vertex.hpp"
#include "iostream"

TEST_CASE("Ensure length works", "[vertex]") {
  REQUIRE(Vertex(1, 2, 3).length2() == Approx(14));
  REQUIRE(Vertex(1, 0, 0).length() == Approx(1));
}

TEST_CASE("Ensure scalar multiplication works", "[vertex]") {
  Vertex v(1, 1, 1);
  Vertex v2(v*2);
  REQUIRE(v2 == Vertex(2, 2, 2));
}

TEST_CASE("Ensure boolean ops work", "[vertex]") {
  Vertex v1(1, 1, 1);
  Vertex v2(1, 1, 1);
  Vertex v3(1, 0, 0);
  REQUIRE(v1 == v2);
  REQUIRE(v1 != v3);
}

TEST_CASE("Ensure normalisation works", "[vertex]") {
  Vertex v(2, 3, 5);
  Vertex v2(v.normalise());
  REQUIRE(v2.length() == Approx(1.0f));
}
