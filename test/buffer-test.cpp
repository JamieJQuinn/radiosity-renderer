#include "catch.hpp"
#include "buffer.hpp"

TEST_CASE("Buffer set and get works", "[buffer]") {
  Buffer<int> buffer(10, 10);
  buffer.set(1, 9, 10);
  REQUIRE( buffer.get(1, 9) == 10 );
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
