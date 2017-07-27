#pragma once

#include <vector>

#include "geometry.hpp"
#include "model.hpp"

Matrix formHemicubeMVP(const Vec3f& eye, const Vec3f& dir, const Vec3f& up, int size);
void calcFormFactorsFromModel(const Model& model, const int faceIdx, std::vector<int>& formFactors, int gridSize);
