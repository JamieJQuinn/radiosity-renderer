#include "catch.hpp"
#include "buffer.hpp"

TEST_CASE("Buffer set and get works", "[buffer]") {
  Buffer<int> buffer(10, 10);
  buffer.set(1, 9, 10);
  REQUIRE( buffer.get(1, 9) == 10 );
}

TEST_CASE("fillLine works", "[buffer]") {
  Buffer<int> buffer (10, 10);
  buffer.fillAll(0);
  buffer.fillLine(1, 5, 6, 1);
  for(int i=0; i<10; ++i) {
    for(int j=0; j<10; ++j) {
      if(j==6 and i>=1 and i<=5) {
        REQUIRE(buffer.get(i, j) == 1);
      } else {
        REQUIRE(buffer.get(i, j) == 0);
      }
    }
  }
}

TEST_CASE("fillAll works", "[buffer]") {
  Buffer<int> buffer (10, 10);
  buffer.fillAll(0);
  for(int i=0; i<10; ++i) {
    for(int j=0; j<10; ++j) {
      REQUIRE(buffer.get(i, j) == 0);
    }
  }
}

TEST_CASE("fillBottomFlatTriangle works", "[buffer]") {
  int solution[] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 1, 0, 0, 0, 0,
  0, 0, 0, 0, 1, 1, 0, 0, 0, 0,
  0, 0, 0, 0, 1, 1, 0, 0, 0, 0,
  0, 0, 0, 0, 1, 1, 0, 0, 0, 0,
  0, 0, 0, 1, 1, 1, 1, 0, 0, 0,
  0, 0, 0, 1, 1, 1, 1, 0, 0, 0,
  0, 0, 0, 1, 1, 1, 1, 0, 0, 0,
  0, 0, 0, 1, 1, 1, 1, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0
  };

  Buffer<int> buffer (10, 10);
  buffer.fillAll(0);
  Vertex v1(5, 1);
  Vertex v2(7, 8);
  Vertex v3(3, 8);
  buffer.fillTriangle(v1, v2, v3, 1);

  for(int j=0; j<10; ++j) {
    for(int i=0; i<10; ++i) {
      REQUIRE(buffer.get(i, j) == solution[j*10+i]);
    }
  }
}

TEST_CASE("fillTopFlatTriangle works", "[buffer]") {
  int solution[] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 1, 1, 1, 1, 0, 0, 0,
  0, 0, 0, 1, 1, 1, 1, 0, 0, 0,
  0, 0, 0, 1, 1, 1, 1, 0, 0, 0,
  0, 0, 0, 0, 1, 1, 0, 0, 0, 0,
  0, 0, 0, 0, 1, 1, 0, 0, 0, 0,
  0, 0, 0, 0, 1, 1, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 1, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0
  };

  Buffer<int> buffer (10, 10);
  buffer.fillAll(0);
  Vertex v1(3, 1);
  Vertex v2(7, 1);
  Vertex v3(5, 8);
  buffer.fillTriangle(v1, v2, v3, 1);

  for(int j=0; j<10; ++j) {
    for(int i=0; i<10; ++i) {
      REQUIRE(buffer.get(i, j) == solution[j*10+i]);
    }
  }
}

TEST_CASE("fillTriangle works", "[buffer]") {
  int solution[] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 1, 0, 0, 0,
  0, 0, 0, 0, 0, 1, 1, 1, 0, 0,
  0, 0, 0, 0, 0, 1, 1, 1, 1, 0,
  0, 0, 0, 0, 0, 1, 1, 1, 0, 0,
  0, 0, 0, 0, 0, 1, 1, 0, 0, 0,
  0, 0, 0, 0, 0, 1, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0
  };

  Buffer<int> buffer (10, 10);
  buffer.fillAll(0);
  Vertex v1(6, 3);
  Vertex v2(8, 5);
  Vertex v3(5, 8);
  buffer.fillTriangle(v1, v2, v3, 1);

  for(int j=0; j<10; ++j) {
    for(int i=0; i<10; ++i) {
      REQUIRE(buffer.get(i, j) == solution[j*10+i]);
    }
  }
}
