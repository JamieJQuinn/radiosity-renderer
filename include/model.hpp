#pragma once

#include <vector>
#include "geometry.hpp"

class Model {
private:
  std::vector<Vec3f> verts_;
  std::vector<Vec3f> norms_;
  std::vector<Vec3f> uv_;
  std::vector<std::vector<Vec3i> > faces_;
public:
  Model(const char *filename);
  ~Model();
  int nverts() const;
  int nfaces() const;
  const Vec3f vert(int i) const;
  const Vec3f norm(int i, int j) const;
  const Vec3f uv(int iface, int nvert) const;
  const std::vector<Vec3i> face(int idx) const;
};
