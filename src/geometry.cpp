#include "geometry.hpp"
#include <cassert>

Matrix::Matrix(int r, int c, float fillValue) {
  m = new float[r*c];
  rows = r;
  cols = c;
  setAll(fillValue);
}

Matrix::~Matrix() {
  delete [] m;
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

Vec4f applyProjection(const Matrix& matrix, const Vec3f& v) {
  return matrix*v2m(v);
}

Matrix formTranslation(const Vec3f& translationVector) {
  Matrix translation = Matrix::identity(4);
  for(int i=0; i<3; ++i) {
    translation.set(i, 3, translationVector[i]);
  }
  return translation;
}

Matrix viewportRelative(int x, int y, int w, int h, int depth) {
  Matrix m = Matrix::identity(4);
  m.set(0, 3, x+w/2.f);
  m.set(1, 3, y+h/2.f);
  m.set(2, 3, depth/2.f);

  m.set(0, 0, w/2.f);
  m.set(1, 1, h/2.f);
  m.set(2, 2, depth/2.f);
  return m;
}

Matrix lookAt(Vec3f eye, Vec3f centre, Vec3f up) {
  Vec3f z = (eye-centre).normalise();
  Vec3f x = up.cross(z).normalise();
  Vec3f y = z.cross(x).normalise();
  Matrix Minv = Matrix::identity();
  for (int i=0; i<3; i++) {
    Minv.set(0, i, x[i]);
    Minv.set(1, i, y[i]);
    Minv.set(2, i, z[i]);
  }
  return Minv;
}

Vec3f getBarycentricCoords(const Vec3f& A, const Vec3f& B, const Vec3f& C, const Vec3f& P) {
  Vec3f u = Vec3f(B.x - A.x, C.x - A.x, A.x - P.x).cross(Vec3f(B.y - A.y, C.y - A.y, A.y - P.y));
  if (std::abs(u.z)<1) return Vec3f(-1,1,1); // triangle is degenerate, in this case return smth with negative coordinates
  return Vec3f(1.f-(u.x+u.y)/u.z, u.x/u.z, u.y/u.z);
}

Vec3f calcNormal(const Vec3f& v1, const Vec3f& v2, const Vec3f& v3) {
  // right handed normal
  return (v2-v1).cross(v3-v1).normalise();
}

Matrix::Matrix(const Vec2f& v):
  rows(2),
  cols(1)
{
  m = new float[2];
  for(int i=0; i<rows; ++i) {
    set(i, 0, v[i]);
  }
}

Matrix::Matrix(const Vec3f& v):
  rows(3),
  cols(1)
{
  m = new float[3];
  for(int i=0; i<rows; ++i) {
    set(i, 0, v[i]);
  }
}

Matrix::Matrix(const Vec4f& v):
  rows(4),
  cols(1)
{
  m = new float[4];
  for(int i=0; i<rows; ++i) {
    set(i, 0, v[i]);
  }
}

Vec2f Matrix::operator*(const Vec2f& v) const {
  return Vec2f(
      get(0,0)*v[0] + get(0,1)*v[1],
      get(1,0)*v[0] + get(1,1)*v[1]
      );
}

Vec3f Matrix::operator*(const Vec3f& v) const {
  return Vec3f(
      get(0,0)*v[0] + get(0,1)*v[1] + get(0,2)*v[2],
      get(1,0)*v[0] + get(1,1)*v[1] + get(1,2)*v[2],
      get(2,0)*v[0] + get(2,1)*v[1] + get(2,2)*v[2]
      );
}

Vec4f Matrix::operator*(const Vec4f& v) const {
  return Vec4f(
      get(0,0)*v[0] + get(0,1)*v[1] + get(0,2)*v[2] + get(0,3)*v[3],
      get(1,0)*v[0] + get(1,1)*v[1] + get(1,2)*v[2] + get(1,3)*v[3],
      get(2,0)*v[0] + get(2,1)*v[1] + get(2,2)*v[2] + get(2,3)*v[3],
      get(3,0)*v[0] + get(3,1)*v[1] + get(3,2)*v[2] + get(3,3)*v[3]
      );
}

float calcTriangleArea(const Vec3f& v1, const Vec3f& v2, const Vec3f& v3) {
  Vec3f u = v2 - v1;
  Vec3f v = v3 - v1;;
  return 0.5f*(u.cross(v).norm());
}
