#pragma once

class Face {
    std::vector<Vec3i> verts;
  public:
    int matIdx;
    int size() const { return verts.size(); }
    void push_back(int ivert, int iuv, int inorm) {verts.push_back(Vec3i(ivert, iuv, inorm));}
    Vec3i& operator[](int i) {return verts[i];}
    const Vec3i& operator[](int i) const {return verts[i];}
};
