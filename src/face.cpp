#include "face.hpp"

Face::Face(const Face& f):
  verts(f.verts),
  matIdx(f.matIdx)
{}

Face::Face():
  matIdx(0)
{}
