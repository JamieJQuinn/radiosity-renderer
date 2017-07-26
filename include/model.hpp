#pragma once

#include <vector>
#include "geometry.hpp"
#include "material.hpp"
#include "face.hpp"

class Model {
private:
  std::vector<Vec3f> verts_;
  std::vector<Vec3f> norms_;
  std::vector<Vec3f> uv_;
  std::vector<Material> materials_;
  std::vector<Face> faces_;
public:
  Model(const char *objFilename, const char *mtlFilename = "");
  ~Model();
  int nverts() const;
  int nfaces() const;
  const Vec3f vert(int i) const;
  const Vec3f norm(int i, int j) const;
  const Vec3f uv(int iface, int nvert) const;
  const Material& material(int iface) const;
  const Face face(int idx) const;
};
