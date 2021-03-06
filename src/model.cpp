#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>

#include "model.hpp"
#include "material.hpp"
#include "face.hpp"

Model::Model(const char *objFilename, const char *mtlFilename) : verts_(), faces_() {
  std::ifstream in;

  // Setup materials
  int materialsCount = 0;
  std::unordered_map<std::string, int> materialsTable;
  in.open (mtlFilename, std::ifstream::in);
  if (in.fail()) return;
  std::string line;
  while (!in.eof()) {
    std::getline(in, line);
    std::istringstream iss(line.c_str());
    char trash;
    if (!line.compare(0, 6, "newmtl")) {
      char trash6[6];
      iss >> trash6;
      std::string matName;
      iss >> matName;
      materialsTable.insert({matName, materialsCount});
      materials_.push_back(Material());
      materialsCount++;
    } else if (!line.compare(0, 3, "Kd ")) {
      iss >> trash >> trash;
      float r, g, b;
      iss >> r >> g >> b;
      materials_[materials_.size()-1].reflectivity = Vec3f(r, g, b);
    } else if (!line.compare(0, 3, "Ke ")) {
      iss >> trash >> trash;
      float r, g, b;
      iss >> r >> g >> b;
      materials_[materials_.size()-1].emissivity = Vec3f(r, g, b);
    }
  }
  in.close();

  in.open (objFilename, std::ifstream::in);
  if (in.fail()) return;
  int matIdx = 0;
  while (!in.eof()) {
    std::getline(in, line);
    std::istringstream iss(line.c_str());
    char trash;
    if (!line.compare(0, 2, "v ")) {
      iss >> trash;
      Vec3f v;
      for (int i=0;i<3;i++) iss >> v.raw[i];
      verts_.push_back(v);
    } else if (!line.compare(0, 3, "vn ")) {
      iss >> trash >> trash;
      Vec3f n;
      for (int i=0;i<3;i++) iss >> n[i];
      n.normalise();
      norms_.push_back(n);
    } else if (!line.compare(0, 6, "usemtl")) {
      char trash6[6];
      iss >> trash6;
      std::string matName;
      iss >> matName;
      matIdx = materialsTable[matName];
    } else if (!line.compare(0, 3, "vt ")) {
      iss >> trash >> trash;
      Vec3f uv;
      for (int i=0;i<2;i++) iss >> uv[i];
      uv_.push_back(uv);
    } else if (!line.compare(0, 2, "f ")) {
      Face f;
      f.matIdx = matIdx;
      int iuv, inorm, ivert;
      iss >> trash;
      while (iss >> ivert >> trash >> iuv >> trash >> inorm) {
        ivert--; // in wavefront obj all indices start at 1, not zero
        iuv--;
        inorm--;
        f.push_back(ivert, iuv, inorm);
      }
      faces_.push_back(f);
    }
  }
  for(int i=0; i<nfaces(); ++i) {
    Face& f = face(i);
    f.area = calcTriangleArea(
      vert(f[0].ivert),
      vert(f[1].ivert),
      vert(f[2].ivert)
      );
  }
}

Model::~Model() {
}

Vec3f Model::getFaceReflectivity(int faceIdx) const {
  return material(face(faceIdx).matIdx).reflectivity;
}

Vec3f Model::getFaceEmissivity(int faceIdx) const {
  return material(face(faceIdx).matIdx).emissivity;
}

TGAColor Model::getFaceColour(const Face& face) const {
  Vec3f matColour = material(face.matIdx).reflectivity*255.0f;
  return TGAColor(matColour.r, matColour.g, matColour.b, 255);
}

TGAColor Model::getFaceColour(int faceIdx) const {
  return getFaceColour(face(faceIdx));
}

const int Model::nverts() const {
  return (int)verts_.size();
}

const int Model::nfaces() const {
  return (int)faces_.size();
}

Face& Model::face(int idx) {
  assert(idx < nfaces());
  return faces_[idx];
}

const Face& Model::face(int idx) const {
  assert(idx < nfaces());
  return faces_[idx];
}

Vec3f Model::vert(int i) const {
  assert(i < nverts());
  return verts_[i];
}

Vec3f Model::norm(int iface, int nvert) const {
  assert(iface < nfaces());
  assert(nvert < nverts());
  int idx = faces_[iface][nvert].inorm;
  return norms_[idx];
}

Vec3f Model::uv(int iface, int nvert) const {
  assert(iface < nfaces());
  assert(nvert < nverts());
  int idx = faces_[iface][nvert].iuv;
  return uv_[idx];
}

const Material& Model::material(int idx) const {
  assert(idx < (int)materials_.size());
  return materials_[idx];
}

Vec3f Model::centreOf(int faceIdx) const {
  assert(faceIdx < nfaces());
  Face f = face(faceIdx);
  int size = f.size();
  Vec3f total(0, 0, 0);
  for(int i=0; i<size; ++i) {
    total = total + vert(f[i].ivert);
  }
  return total*(1.f/size);
}

float Model::area(int faceIdx) const {
  return face(faceIdx).area;
}
