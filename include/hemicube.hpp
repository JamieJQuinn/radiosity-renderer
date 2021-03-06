#pragma once

#include <vector>

#include "geometry.hpp"
#include "model.hpp"
#include "buffer.hpp"

Matrix formHemicubeMVP(const Vec3f& eye, const Vec3f& dir, const Vec3f& up);

void renderHemicube(Buffer<unsigned int>& buffer, const Model& model, int faceIdx, const Vec3f& eye, const Vec3f& dir, const Vec3f& up);
void renderToHemicube(Buffer<unsigned int>& mainBuffer, const Model& model, int faceIdx);

void calcFormFactorPerCell(const int sideLengthInPixels, Buffer<float>& topFace, Buffer<float>& sideFace);
void calcFormFactorsFromBuffer(const Buffer<unsigned int>& itemBuffer, const Buffer<float>& factorsPerCell, float* formFactors);
void calcFormFactorsFromSideBuffer(const Buffer<unsigned int>& itemBuffer, const Buffer<float>& factorsPerCell, float* formFactors);
void calcFormFactorsWholeModel(const Model& model, Buffer<float>& formFactors, int gridSize);
void calcFormFactorsSingleFace(const Model& model, const int faceIdx, float* formFactors, int gridSize, const Buffer<float>& topFace, const Buffer<float>& sideFace);

Vec3f getUp(const Vec3f& dir);
