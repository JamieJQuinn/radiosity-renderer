#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include "model.hpp"

Model::Model(const char *filename) : verts_(), faces_() {
  std::ifstream in;
  in.open (filename, std::ifstream::in);
  if (in.fail()) return;
  std::string line;
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
    } else if (!line.compare(0, 3, "vt ")) {
      iss >> trash >> trash;
      Vec3f uv;
      for (int i=0;i<2;i++) iss >> uv[i];
      uv_.push_back(uv);
    } else if (!line.compare(0, 2, "f ")) {
      std::vector<Vec3i> f;
      int iuv, inorm, ivert;
      iss >> trash;
      while (iss >> ivert >> trash >> iuv >> trash >> inorm) {
        ivert--; // in wavefront obj all indices start at 1, not zero
        iuv--;
        inorm--;
        f.push_back(Vec3i(ivert, iuv, inorm));
      }
      if(f.size() == 4) {
        std::swap(f[2], f[3]);
      }
      faces_.push_back(f);
    }
  }
  //std::cerr << "# v# " << verts_.size() << " f# "  << faces_.size() << std::endl;
}

Model::~Model() {
}

int Model::nverts() const {
  return (int)verts_.size();
}

int Model::nfaces() const {
  return (int)faces_.size();
}

const std::vector<Vec3i> Model::face(int idx) const {
  return faces_[idx];
}

const Vec3f Model::vert(int i) const {
  return verts_[i];
}

const Vec3f Model::norm(int iface, int nvert) const {
    int idx = faces_[iface][nvert].inorm;
    return norms_[idx];
}

const Vec3f Model::uv(int iface, int nvert) const {
    int idx = faces_[iface][nvert].iuv;
    return uv_[idx];
}
