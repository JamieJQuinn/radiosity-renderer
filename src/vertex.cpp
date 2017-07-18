#include <iostream>
#include "vertex.hpp"

std::ostream& operator<<(std::ostream & stream, const Vertex & v){
  stream << v.x << ", " << v.y << ", " << v.z;
  return stream;
}
