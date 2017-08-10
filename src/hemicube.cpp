#include "hemicube.hpp"
#include "rendering.hpp"
#include "geometry.hpp"
#include "face.hpp"
#include "model.hpp"
#include "buffer.hpp"
#include "rendering.hpp"

Matrix formHemicubeMVP(const Vec3f& eye, const Vec3f& dir, const Vec3f& up) {
  Matrix translation = formTranslation(eye*-1);
  Matrix view = lookAt(Vec3f(0, 0, 0), dir, up)*translation;
  Matrix projection = formRightAngledProjection(0.05f, 20.0f);
  return projection*view;
}

void renderHemicubeFront(Buffer<int>& mainBuffer, const Model& model, int faceIdx) {
  int gridSize = mainBuffer.width/2;
  Face f = model.face(faceIdx);
  Vec3f up = (model.vert(f[1].ivert) - model.vert(f[0].ivert)).normalise();
  Vec3f dir = model.norm(faceIdx, 0);
  Vec3f eye = model.centreOf(faceIdx);

  Matrix MVP = formHemicubeMVP(eye, dir, up);
  Buffer<int> buffer(gridSize, gridSize, 0);
  renderModelIds(buffer, model, MVP, dir, 0.05f);

  for(int j=0; j<gridSize; ++j) {
    for(int i=0; i<gridSize; ++i) {
      mainBuffer.set(gridSize/2+i, gridSize/2+j, buffer.get(i, j));
    }
  }
}

void renderHemicubeUp(Buffer<int>& mainBuffer, const Model& model, int faceIdx) {
  int gridSize = mainBuffer.width/2;
  Face f = model.face(faceIdx);
  Vec3f up = (model.vert(f[1].ivert) - model.vert(f[0].ivert)).normalise();
  Vec3f dir = model.norm(faceIdx, 0);
  Vec3f eye = model.centreOf(faceIdx);

  std::swap(up, dir);

  Matrix MVP = formHemicubeMVP(eye, dir, up);
  Buffer<int> buffer(gridSize, gridSize, 0);
  renderModelIds(buffer, model, MVP, dir, 0.05f);

  for(int j=0; j<gridSize/2; ++j) {
    for(int i=0; i<gridSize; ++i) {
      mainBuffer.set(gridSize/2+i, j+3*gridSize/2, buffer.get(gridSize-i-1, j+gridSize/2));
    }
  }
}

void renderHemicubeDown(Buffer<int>& mainBuffer, const Model& model, int faceIdx) {
  int gridSize = mainBuffer.width/2;
  Face f = model.face(faceIdx);
  Vec3f up = (model.vert(f[1].ivert) - model.vert(f[0].ivert)).normalise();
  Vec3f dir = model.norm(faceIdx, 0);
  Vec3f eye = model.centreOf(faceIdx);

  std::swap(up, dir);
  dir = dir*-1.f;

  Matrix MVP = formHemicubeMVP(eye, dir, up);
  Buffer<int> buffer(gridSize, gridSize, 0);
  renderModelIds(buffer, model, MVP, dir, 0.05f);

  for(int j=0; j<gridSize/2; ++j) {
    for(int i=0; i<gridSize; ++i) {
      mainBuffer.set(gridSize/2+i, j, buffer.get(i, j+gridSize/2));
    }
  }
}

void renderHemicubeRight(Buffer<int>& mainBuffer, const Model& model, int faceIdx) {
  int gridSize = mainBuffer.width/2;
  Face f = model.face(faceIdx);
  Vec3f up = (model.vert(f[1].ivert) - model.vert(f[0].ivert)).normalise();
  Vec3f dir = model.norm(faceIdx, 0);
  Vec3f eye = model.centreOf(faceIdx);

  std::swap(up, dir);
  dir = dir.cross(up)*-1.f;

  Matrix MVP = formHemicubeMVP(eye, dir, up);
  Buffer<int> buffer(gridSize, gridSize, 0);
  renderModelIds(buffer, model, MVP, dir, 0.05f);

  for(int j=0; j<gridSize/2; ++j) {
    for(int i=0; i<gridSize; ++i) {
      mainBuffer.set(j+3*gridSize/2, i+gridSize/2, buffer.get(i, j+gridSize/2));
    }
  }
}

void renderHemicubeLeft(Buffer<int>& mainBuffer, const Model& model, int faceIdx) {
  int gridSize = mainBuffer.width/2;
  Face f = model.face(faceIdx);
  Vec3f up = (model.vert(f[1].ivert) - model.vert(f[0].ivert)).normalise();
  Vec3f dir = model.norm(faceIdx, 0);
  Vec3f eye = model.centreOf(faceIdx);

  std::swap(up, dir);
  dir = dir.cross(up);

  Matrix MVP = formHemicubeMVP(eye, dir, up);
  Buffer<int> buffer(gridSize, gridSize, 0);
  renderModelIds(buffer, model, MVP, dir, 0.05f);

  for(int j=0; j<gridSize/2; ++j) {
    for(int i=0; i<gridSize; ++i) {
      mainBuffer.set(j, i+gridSize/2, buffer.get(gridSize-i-1, gridSize/2+j));
    }
  }
}

void renderToHemicube(Buffer<int>& mainBuffer, const Model& model, int faceIdx) {
  renderHemicubeFront(mainBuffer, model, faceIdx);
  renderHemicubeUp(mainBuffer, model, faceIdx);
  renderHemicubeDown(mainBuffer, model, faceIdx);
  renderHemicubeLeft(mainBuffer, model, faceIdx);
  renderHemicubeRight(mainBuffer, model, faceIdx);
}

// TODO
//void calcFormFactorsFromModel(const Model& model, const int faceIdx, std::vector<float>& formFactors, int gridSize) {
  //Face f = model.face(faceIdx);
  //Vec3f eye = model.centreOf(faceIdx);
  //Vec3f dir = model.norm(faceIdx, 0);
  //Vec3f up = model.vert(f[1].ivert) - model.vert(f[0].ivert);

  //Matrix MVP = formHemicubeMVP(eye, dir, up, gridSize);

  //Buffer<int> itemBuffer(gridSize, gridSize, 0);

  ////renderModel(itemBuffer, model, MVP);

  //Buffer<float> topFace(gridSize, gridSize, 0);
  //Buffer<float> sideFace(gridSize, gridSize, 0);
  //calcFormFactorPerCell(gridSize, topFace, sideFace);
  //calcFormFactorsFromBuffer(itemBuffer, topFace, formFactors);

  //// TODO side faces
  //// For each side of hemicube:
    //// Form view (MVP)
    //// Render view to itemBuffer
    //// calc form factors from itemBuffer
//}
