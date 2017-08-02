#include "geometry.hpp"
#include <cassert>

Matrix::Matrix(int r, int c, float fillValue) {
  m = new float[r*c];
  rows = r;
  cols = c;
  setAll(fillValue);
}

void Matrix::setAll(float f) {
  for(int j=0; j<cols; ++j) {
    for(int i=0; i<rows; ++i) {
      set(i, j, f);
    }
  }
}

Matrix Matrix::identity(int dimensions) {
  Matrix result(dimensions, dimensions);
  for(int i=0; i<dimensions; ++i) {
    result.set(i, i, 1.f);
  }
  return result;
}

inline float Matrix::get(int i, int j) const {
  return m[j*cols + i];
}

inline void Matrix::set(int i, int j, float value) {
  m[j*cols + i] = value;
}

Matrix Matrix::transpose() {
  Matrix result(cols, rows);
  for(int i=0; i<rows; i++)
    for(int j=0; j<cols; j++)
      result.set(i, j, get(j, i));
  return result;
}

Matrix Matrix::operator*(const Matrix& a) const {
  assert(cols == a.rows);
  Matrix result(rows, a.cols);
  for (int i=0; i<rows; i++) {
    for (int j=0; j<a.cols; j++) {
      result.set(i, j, 0.f);
      for (int k=0; k<cols; k++) {
        result.set(i, j, result.get(i, j) + get(i, k)*a.get(k, j));
      }
    }
  }
  return result;
}

std::ostream& operator<<(std::ostream& s, Matrix& m) {
  for (int i=0; i<m.nrows(); i++)  {
    for (int j=0; j<m.ncols(); j++) {
      s << m.get(i, j);
      if (j<m.ncols()-1) s << "\t";
    }
    s << "\n";
  }
  return s;
}

Vec3f m2v(const Matrix& m) {
  return Vec3f(m.get(0, 0), m.get(1, 0), m.get(2, 0))*(1.f/m.get(3, 0));
}

Matrix v2m(const Vec3f& v) {
  Matrix m(4, 1);
  m.set(0, 0, v.x);
  m.set(1, 0, v.y);
  m.set(2, 0, v.z);
  m.set(3, 0, 1.f);
  return m;
}

Matrix formProjection(float l, float r, float b, float t, float n, float f) {
  Matrix p;
  p.set(0, 0, 2.f*n/(r-l));
  p.set(1, 1, 2.f*n/(t-b));
  p.set(0, 2, (r+l)/(r-l));
  p.set(1, 2, (t+b)/(t-b));
  p.set(2, 2,  (f+n)/(f-n));
  p.set(3, 2, -1);
  p.set(2, 3, 2.f*f*n/(f-n));

  return p;
}

Matrix formRightAngledProjection(float n, float f) {
  return formProjection(-n, n, -n, n, n, f);
}

Vec3f applyTransform(const Matrix& matrix, const Vec3f& v) {
  return m2v(matrix*v2m(v));
}

Matrix formTranslation(const Vec3f& translationVector) {
  Matrix translation = Matrix::identity(4);
  for(int i=0; i<3; ++i) {
    translation.set(i, 3, translationVector[i]);
  }
  return translation;
}

