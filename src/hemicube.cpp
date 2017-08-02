#include "hemicube.hpp"
#include "rendering.hpp"
#include "geometry.hpp"
#include "face.hpp"
#include "model.hpp"
#include "buffer.hpp"
#include "rendering.hpp"

Matrix formHemicubeMVP(const Vec3f& eye, const Vec3f& dir, const Vec3f& up, int size) {
  Matrix translation = formTranslation(eye*-1);
  Matrix view = lookAt(Vec3f(0, 0, 0), dir, up)*translation;
  Matrix projection = formRightAngledProjection(0.05f, 20.0f);
  Matrix viewport = viewportRelative(0, 0, size, size);
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
