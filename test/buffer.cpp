#include "catch.hpp"
#include "buffer.hpp"

TEST_CASE("Buffer set and get works", "[buffer]") {
  Buffer<int> buffer(10, 10);
  buffer.set(1, 9, 10);
  REQUIRE( buffer.get(1, 9) == 10 );
}
