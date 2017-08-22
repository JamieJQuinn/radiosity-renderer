#include "hemicube.hpp"
#include "rendering.hpp"
#include "geometry.hpp"
#include "face.hpp"
#include "model.hpp"
#include "buffer.hpp"
#include "rendering.hpp"
#include "opengl_helper.hpp"
#include "opengl.hpp"
#include "config.hpp"

#include <omp.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

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

void calcFormFactorsFromBuffer(const Buffer<unsigned int>& itemBuffer, const Buffer<float>& factorsPerCell, float* formFactors) {
  for(int j=0; j<factorsPerCell.height; ++j) {
    for(int i=0; i<factorsPerCell.width; ++i) {
      int idx = itemBuffer.get(i, j);
      formFactors[idx] += factorsPerCell.get(i, j);
    }
  }
}

void calcFormFactorsFromSideBuffer(const Buffer<unsigned int>& itemBuffer, const Buffer<float>& factorsPerCell, float* formFactors) {
  for(int j=0; j<factorsPerCell.height; ++j) {
    for(int i=0; i<factorsPerCell.width; ++i) {
      int idx = itemBuffer.get(i, j+itemBuffer.height/2);
      formFactors[idx] += factorsPerCell.get(i, j);
    }
  }
}

Matrix formHemicubeMVP(const Vec3f& eye, const Vec3f& dir, const Vec3f& up) {
  Matrix translation = formTranslation(eye*-1);
  Matrix view = lookAt(Vec3f(0, 0, 0), dir, up)*translation;
  Matrix projection = formRightAngledProjection(HEMICUBE_NEAR_PLANE, 20.0f);
  return projection*view;
}

Vec3f getUp(const Vec3f& dir) {
  if(std::abs(dir.z) < 0.98f) {
    return Vec3f(0,0,1);
  } else {
    return Vec3f(1,0,0);
  }
}

void renderHemicube(Buffer<unsigned int>& buffer, const Model& model, int faceIdx, const Vec3f& eye, const Vec3f& dir, const Vec3f& up) {
#ifdef OPENGL
  glm::mat4 CameraMatrix = glm::lookAt(
      glmVec3FromVec3f(eye),
      glmVec3FromVec3f(eye + dir),
      glmVec3FromVec3f(up));
  glm::mat4 Projection = glm::perspective(glm::radians(90.0f), 1.f, HEMICUBE_NEAR_PLANE, 20.0f);
  glm::mat4 MVP = Projection*CameraMatrix;

  extern OpenGLRenderer * renderer;
  renderer->renderHemicube(buffer, MVP);
#else
  Matrix MVP = formHemicubeMVP(eye, dir, up);
  renderModelIds(buffer, model, MVP, eye, 0.05f);
#endif
}

void renderToHemicube(Buffer<unsigned int>& mainBuffer, const Model& model, int faceIdx) {
  int gridSize = mainBuffer.width/2;
  Buffer<unsigned int> buffer(gridSize, gridSize, 0);

  Face f = model.face(faceIdx);
  Vec3f dir = model.norm(faceIdx, 0);
  Vec3f up = getUp(dir);
  Vec3f eye = model.centreOf(faceIdx);

  renderHemicube(buffer, model, faceIdx, eye, dir, up);
  for(int j=0; j<gridSize; ++j) {
    for(int i=0; i<gridSize; ++i) {
      mainBuffer.set(gridSize/2+i, gridSize/2+j, buffer.get(i, j));
    }
  }

  buffer.fillAll(0);
  std::swap(up, dir);
  renderHemicube(buffer, model, faceIdx, eye, dir, up);
  for(int j=0; j<gridSize/2; ++j) {
    for(int i=0; i<gridSize; ++i) {
      mainBuffer.set(gridSize/2+i, j+3*gridSize/2, buffer.get(gridSize-i-1, gridSize-j-1));
    }
  }

  buffer.fillAll(0);
  dir = dir*-1.f;
  renderHemicube(buffer, model, faceIdx, eye, dir, up);
  for(int j=0; j<gridSize/2; ++j) {
    for(int i=0; i<gridSize; ++i) {
      mainBuffer.set(gridSize/2+i, j, buffer.get(i, j+gridSize/2));
    }
  }

  buffer.fillAll(0);
  dir = dir.cross(up)*-1.f;
  renderHemicube(buffer, model, faceIdx, eye, dir, up);
  for(int j=0; j<gridSize/2; ++j) {
    for(int i=0; i<gridSize; ++i) {
      mainBuffer.set(j, i+gridSize/2, buffer.get(gridSize-i-1, gridSize/2+j));
    }
  }

  buffer.fillAll(0);
  dir = dir*-1.f;
  renderHemicube(buffer, model, faceIdx, eye, dir, up);
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

#ifndef OPENGL
  #pragma omp parallel for
#endif
  for(int i=0; i<model.nfaces(); ++i) {
    calcFormFactorsSingleFace(model, i, formFactors.getRow(i), gridSize, topFace, sideFace);
  }
}

void calcFormFactorsSingleFace(const Model& model, const int faceIdx, float* formFactors, int gridSize, const Buffer<float>& topFace, const Buffer<float>& sideFace) {
  assert(gridSize%2 == 0);

  Face f = model.face(faceIdx);
  Vec3f dir = model.norm(faceIdx, 0);
  Vec3f up = getUp(dir);
  Vec3f eye = model.centreOf(faceIdx);

  Buffer<unsigned int> itemBuffer(gridSize, gridSize, 0);

  std::swap(up, dir);
  renderHemicube(itemBuffer, model, faceIdx, eye, dir, up);
  calcFormFactorsFromSideBuffer(itemBuffer, sideFace, formFactors);

  dir = dir*-1.f;
  renderHemicube(itemBuffer, model, faceIdx, eye, dir, up);
  calcFormFactorsFromSideBuffer(itemBuffer, sideFace, formFactors);

  dir = dir.cross(up);
  renderHemicube(itemBuffer, model, faceIdx, eye, dir, up);
  calcFormFactorsFromSideBuffer(itemBuffer, sideFace, formFactors);

  dir = dir*-1.f;
  renderHemicube(itemBuffer, model, faceIdx, eye, dir, up);
  calcFormFactorsFromSideBuffer(itemBuffer, sideFace, formFactors);

  std::swap(up, dir);
  renderHemicube(itemBuffer, model, faceIdx, eye, dir, up);
  calcFormFactorsFromBuffer(itemBuffer, topFace, formFactors);
}
