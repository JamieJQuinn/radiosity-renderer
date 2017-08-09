#pragma once

#include <vector>
#include "geometry.hpp"
#include "material.hpp"
#include "face.hpp"
#include "tgaimage.hpp"

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
  // TODO fix names
  int nverts() const;
  int nfaces() const;
  // TODO cleanup these functions - make consistent
  Vec3f vert(int i) const;
  Vec3f norm(int i, int j) const;
  Vec3f uv(int iface, int nvert) const;
  const Material& material(int iface) const;
  const Face face(int idx) const;
  Vec3f centreOf(int faceIdx) const;
  TGAColor getFaceColour(const Face& face) const;
};
