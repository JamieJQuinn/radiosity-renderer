#include "hemicube.hpp"
#include "rendering.hpp"
#include "geometry.hpp"
#include "face.hpp"
#include "model.hpp"
#include "buffer.hpp"
#include "rendering.hpp"

Matrix formHemicubeMVP(const Vec3f& eye, const Vec3f& dir, const Vec3f& up, int size) {
  Vec3f centre(eye-dir);
  Matrix translation = Matrix::identity(4);
  for(int i=0; i<3; ++i) {
    translation.set(i, 3, -eye[i]);
  }
  Matrix view = lookAt(eye, centre, up)*translation;

  Matrix projection = Matrix::identity(4);
  projection.set(3, 2, -1.f/(dir).norm());
  Matrix viewport = viewportRelative(0, 0, size, size, 255);
  return viewport*projection*view;
}

void calcFormFactorsFromModel(const Model& model, const int faceIdx, std::vector<float>& formFactors, int gridSize) {
  Face f = model.face(faceIdx);
  Vec3f eye = model.centreOf(faceIdx);
  Vec3f dir = model.norm(faceIdx, 0);
  Vec3f up = model.vert(f[1].ivert) - model.vert(f[0].ivert);

  Matrix MVP = formHemicubeMVP(eye, dir, up, gridSize);

  Buffer<int> itemBuffer(gridSize, gridSize, 0);

  renderModel(itemBuffer, model, MVP);

  Buffer<float> topFace(gridSize, gridSize, 0);
  Buffer<float> sideFace(gridSize, gridSize, 0);
  calcFormFactorPerCell(gridSize, topFace, sideFace);
  calcFormFactorsFromBuffer(itemBuffer, topFace, formFactors);

  // TODO side faces
  // For each side of hemicube:
    // Form view (MVP)
    // Render view to itemBuffer
    // calc form factors from itemBuffer
}
