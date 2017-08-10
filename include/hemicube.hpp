#pragma once

#include <vector>

#include "geometry.hpp"
#include "model.hpp"
#include "buffer.hpp"

Matrix formHemicubeMVP(const Vec3f& eye, const Vec3f& dir, const Vec3f& up);

void renderToHemicube(Buffer<int>& mainBuffer, const Model& model, int faceIdx);
void renderHemicubeFront(Buffer<int>& mainBuffer, const Model& model, int faceIdx);
void renderHemicubeDown(Buffer<int>& mainBuffer, const Model& model, int faceIdx);
void renderHemicubeUp(Buffer<int>& mainBuffer, const Model& model, int faceIdx);
void renderHemicubeRight(Buffer<int>& mainBuffer, const Model& model, int faceIdx);
void renderHemicubeLeft(Buffer<int>& mainBuffer, const Model& model, int faceIdx);

void calcFormFactorPerCell(const int sideLengthInPixels, Buffer<float>& topFace, Buffer<float>& sideFace);
void calcFormFactorsFromBuffer(const Buffer<int>& itemBuffer, const Buffer<float>& factorsPerCell, std::vector<float>& formFactors);
void calcFormFactorsFromSideBuffer(const Buffer<int>& itemBuffer, const Buffer<float>& factorsPerCell, std::vector<float>& formFactors);
void calcFormFactorsFromModel(const Model& model, const int faceIdx, std::vector<float>& formFactors, int gridSize);
