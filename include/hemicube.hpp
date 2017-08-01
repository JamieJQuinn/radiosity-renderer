#pragma once

#include <vector>

#include "geometry.hpp"
#include "model.hpp"

Matrix formHemicubeMVP(const Vec3f& eye, const Vec3f& dir, const Vec3f& up, int size);
void calcFormFactorsFromModel(const Model& model, const int faceIdx, std::vector<float>& formFactors, int gridSize);
Matrix formProjection(float l, float r, float b, float t, float n, float f);
