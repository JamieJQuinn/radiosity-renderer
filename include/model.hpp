#pragma once

#include <vector>
#include <GL/gl.h>

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
#ifdef OPENGL
  GLfloat * vertex_buffer_data;
  GLfloat * colour_buffer_data;
  GLuint * id_buffer_data;
#endif
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
  Face& face(int idx);
  const Face& face(int idx) const;
  Vec3f centreOf(int faceIdx) const;
  TGAColor getFaceColour(const Face& face) const;
  TGAColor getFaceColour(int faceIdx) const;
  float area(int faceIdx) const;
  Vec3f getFaceReflectivity(int faceIdx) const;
  Vec3f getFaceEmissivity(int faceIdx) const;

#ifdef OPENGL
  void initVertexBuffer();
  void initColourBuffer();
  void initIndexBuffer();
  GLfloat * getVertexBuffer();
  GLfloat * getColourBuffer();
  GLuint * getIndexBuffer();
#endif
};
