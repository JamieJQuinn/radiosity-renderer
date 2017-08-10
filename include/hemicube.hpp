#pragma once

#include <vector>

#include "geometry.hpp"
#include "model.hpp"
#include "buffer.hpp"

Matrix formHemicubeMVP(const Vec3f& eye, const Vec3f& dir, const Vec3f& up);
void calcFormFactorsFromModel(const Model& model, const int faceIdx, std::vector<float>& formFactors, int gridSize);
void renderToHemicube(Buffer<int>& mainBuffer, const Model& model, int faceIdx);
void renderHemicubeFront(Buffer<int>& mainBuffer, const Model& model, int faceIdx);
void renderHemicubeDown(Buffer<int>& mainBuffer, const Model& model, int faceIdx);
void renderHemicubeUp(Buffer<int>& mainBuffer, const Model& model, int faceIdx);
void renderHemicubeRight(Buffer<int>& mainBuffer, const Model& model, int faceIdx);
void renderHemicubeLeft(Buffer<int>& mainBuffer, const Model& model, int faceIdx);
