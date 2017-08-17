#include "hemicube.hpp"
#include "rendering.hpp"
#include "geometry.hpp"
#include "face.hpp"
#include "model.hpp"
#include "buffer.hpp"
#include "rendering.hpp"
#include "omp.h"

void calcFormFactorPerCell(const int sideLengthInPixels, Buffer<float>& topFace, Buffer<float>& sideFace) {
  assert(sideLengthInPixels%2==0); // Need to half for side face

  float pixelLength = 2.f/sideLengthInPixels;
  float dA = pixelLength*pixelLength;

  float initialX = -1.f + pixelLength/2.f;
  float initialY = -1.f + pixelLength/2.f;
  for(int j=0; j<sideLengthInPixels; ++j) {
    for(int i=0; i<sideLengthInPixels; ++i) {
      float x = initialX + pixelLength*i;
      float y = initialY + pixelLength*j;
      float r2 = x*x + y*y + 1.f;
      float factor = dA/(r2*r2*M_PI);
      topFace.set(i, j, factor);
    }
  }

  float initialZ = pixelLength/2.f;
  for(int j=0; j<sideLengthInPixels/2; ++j) {
    for(int i=0; i<sideLengthInPixels; ++i) {
      float z = initialZ + pixelLength*j;
      float y = initialY + pixelLength*i;
      float r2 = z*z + y*y + 1.f;
      float factor = z*dA/(r2*r2*M_PI);
      sideFace.set(i, j, factor);
    }
  }
}

void calcFormFactorsFromBuffer(const Buffer<int>& itemBuffer, const Buffer<float>& factorsPerCell, float* formFactors) {
  for(int j=0; j<factorsPerCell.height; ++j) {
    for(int i=0; i<factorsPerCell.width; ++i) {
      int idx = itemBuffer.get(i, j);
      formFactors[idx] += factorsPerCell.get(i, j);
    }
  }
}

void calcFormFactorsFromSideBuffer(const Buffer<int>& itemBuffer, const Buffer<float>& factorsPerCell, float* formFactors) {
  for(int j=0; j<factorsPerCell.height; ++j) {
    for(int i=0; i<factorsPerCell.width; ++i) {
      int idx = itemBuffer.get(i, itemBuffer.height-j-1);
      formFactors[idx] += factorsPerCell.get(i, j);
    }
  }
}

Matrix formHemicubeMVP(const Vec3f& eye, const Vec3f& dir, const Vec3f& up) {
  Matrix translation = formTranslation(eye*-1);
  Matrix view = lookAt(Vec3f(0, 0, 0), dir, up)*translation;
  Matrix projection = formRightAngledProjection(0.05f, 20.0f);
  return projection*view;
}

void renderHemicubeFront(Buffer<int>& buffer, const Model& model, int faceIdx) {
  Face f = model.face(faceIdx);
  Vec3f up = (model.vert(f[1].ivert) - model.vert(f[0].ivert)).normalise();
  Vec3f dir = model.norm(faceIdx, 0);
  Vec3f eye = model.centreOf(faceIdx);

  Matrix MVP = formHemicubeMVP(eye, dir, up);
  renderModelIds(buffer, model, MVP, eye, 0.05f);
}

void renderHemicubeDown(Buffer<int>& buffer, const Model& model, int faceIdx) {
  Face f = model.face(faceIdx);
  Vec3f up = (model.vert(f[1].ivert) - model.vert(f[0].ivert)).normalise();
  Vec3f dir = model.norm(faceIdx, 0);
  Vec3f eye = model.centreOf(faceIdx);

  std::swap(up, dir);

  Matrix MVP = formHemicubeMVP(eye, dir, up);
  renderModelIds(buffer, model, MVP, eye, 0.05f);
}

void renderHemicubeUp(Buffer<int>& buffer, const Model& model, int faceIdx) {
  Face f = model.face(faceIdx);
  Vec3f up = (model.vert(f[1].ivert) - model.vert(f[0].ivert)).normalise();
  Vec3f dir = model.norm(faceIdx, 0);
  Vec3f eye = model.centreOf(faceIdx);

  std::swap(up, dir);
  dir = dir*-1.f;

  Matrix MVP = formHemicubeMVP(eye, dir, up);
  renderModelIds(buffer, model, MVP, eye, 0.05f);
}

void renderHemicubeRight(Buffer<int>& buffer, const Model& model, int faceIdx) {
  Face f = model.face(faceIdx);
  Vec3f up = (model.vert(f[1].ivert) - model.vert(f[0].ivert)).normalise();
  Vec3f dir = model.norm(faceIdx, 0);
  Vec3f eye = model.centreOf(faceIdx);

  std::swap(up, dir);
  dir = dir.cross(up)*-1.f;

  Matrix MVP = formHemicubeMVP(eye, dir, up);
  renderModelIds(buffer, model, MVP, eye, 0.05f);
}

void renderHemicubeLeft(Buffer<int>& buffer, const Model& model, int faceIdx) {
  Face f = model.face(faceIdx);
  Vec3f up = (model.vert(f[1].ivert) - model.vert(f[0].ivert)).normalise();
  Vec3f dir = model.norm(faceIdx, 0);
  Vec3f eye = model.centreOf(faceIdx);

  std::swap(up, dir);
  dir = dir.cross(up);

  Matrix MVP = formHemicubeMVP(eye, dir, up);
  renderModelIds(buffer, model, MVP, eye, 0.05f);
}

void renderToHemicube(Buffer<int>& mainBuffer, const Model& model, int faceIdx) {
  int gridSize = mainBuffer.width/2;
  Buffer<int> buffer(gridSize, gridSize, 0);

  renderHemicubeFront(buffer, model, faceIdx);
  for(int j=0; j<gridSize; ++j) {
    for(int i=0; i<gridSize; ++i) {
      mainBuffer.set(gridSize/2+i, gridSize/2+j, buffer.get(i, j));
    }
  }

  buffer.fillAll(0);
  renderHemicubeDown(buffer, model, faceIdx);
  for(int j=0; j<gridSize/2; ++j) {
    for(int i=0; i<gridSize; ++i) {
      mainBuffer.set(gridSize/2+i, j+3*gridSize/2, buffer.get(gridSize-i-1, gridSize-j-1));
    }
  }

  buffer.fillAll(0);
  renderHemicubeUp(buffer, model, faceIdx);
  for(int j=0; j<gridSize/2; ++j) {
    for(int i=0; i<gridSize; ++i) {
      mainBuffer.set(gridSize/2+i, j, buffer.get(i, j+gridSize/2));
    }
  }

  buffer.fillAll(0);
  renderHemicubeLeft(buffer, model, faceIdx);
  for(int j=0; j<gridSize/2; ++j) {
    for(int i=0; i<gridSize; ++i) {
      mainBuffer.set(j, i+gridSize/2, buffer.get(gridSize-i-1, gridSize/2+j));
    }
  }

  buffer.fillAll(0);
  renderHemicubeRight(buffer, model, faceIdx);
  for(int j=0; j<gridSize/2; ++j) {
    for(int i=0; i<gridSize; ++i) {
      mainBuffer.set(j+3*gridSize/2, i+gridSize/2, buffer.get(i, gridSize-j-1));
    }
  }
}

void calcFormFactorsWholeModel(const Model& model, Buffer<float>& formFactors, int gridSize) {
  // Precalculate face form factors
  Buffer<float> topFace(gridSize, gridSize, 0);
  Buffer<float> sideFace(gridSize, gridSize/2, 0);
  calcFormFactorPerCell(gridSize, topFace, sideFace);

  #pragma omp parallel for
  for(int i=0; i<model.nfaces(); ++i) {
    calcFormFactorsSingleFace(model, i, formFactors.getRow(i), gridSize, topFace, sideFace);
  }
}

void calcFormFactorsSingleFace(const Model& model, const int faceIdx, float* formFactors, int gridSize, const Buffer<float>& topFace, const Buffer<float>& sideFace) {
  assert(gridSize%2 == 0);

  Buffer<int> itemBuffer(gridSize, gridSize, 0);
  renderHemicubeFront(itemBuffer, model, faceIdx);
  calcFormFactorsFromBuffer(itemBuffer, topFace, formFactors);

  itemBuffer.fillAll(0);
  renderHemicubeUp(itemBuffer, model, faceIdx);
  calcFormFactorsFromSideBuffer(itemBuffer, sideFace, formFactors);

  itemBuffer.fillAll(0);
  renderHemicubeDown(itemBuffer, model, faceIdx);
  calcFormFactorsFromSideBuffer(itemBuffer, sideFace, formFactors);

  itemBuffer.fillAll(0);
  renderHemicubeLeft(itemBuffer, model, faceIdx);
  calcFormFactorsFromSideBuffer(itemBuffer, sideFace, formFactors);

  itemBuffer.fillAll(0);
  renderHemicubeRight(itemBuffer, model, faceIdx);
  calcFormFactorsFromSideBuffer(itemBuffer, sideFace, formFactors);
}
