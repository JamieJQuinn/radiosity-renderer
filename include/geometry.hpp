#pragma once

#include <cmath>
#include <ostream>
#include <cassert>

// Forward declarations
template <class t> struct Vec2;
template <class t> struct Vec3;
template <class t> struct Vec4;

typedef Vec2<float> Vec2f;
typedef Vec2<int>   Vec2i;
typedef Vec3<float> Vec3f;
typedef Vec3<int>   Vec3i;
typedef Vec4<float> Vec4f;
typedef Vec4<int>   Vec4i;

const int DEFAULT_MATRIX_SIZE = 4;

class Matrix {
  float* m;
  int rows, cols;
public:
  Matrix(int r=DEFAULT_MATRIX_SIZE, int c=DEFAULT_MATRIX_SIZE, float fillValue=0.f);
  Matrix(const Vec2f& v);
  Matrix(const Vec3f& v);
  Matrix(const Vec4f& v);
  inline int nrows() const {return rows;}
  inline int ncols() const {return cols;}

  void setAll(float f);
  static Matrix identity(int dimensions=DEFAULT_MATRIX_SIZE);
  float get(int i, int j) const;
  void set(int i, int j, float value);
  Matrix operator*(const Matrix& a) const;
  Vec2f operator*(const Vec2f& v) const;
  Vec3f operator*(const Vec3f& v) const;
  Vec4f operator*(const Vec4f& v) const;
  Matrix transpose();

  friend std::ostream& operator<<(std::ostream& s, Matrix& m);
};

Vec3f m2v(const Matrix& m);
Matrix v2m(const Vec3f& v);

Matrix formTranslation(const Vec3f& translationVector);
Matrix viewportRelative(int x, int y, int w, int h, int depth=1.0f);
Matrix viewportAbsolute(int x0, int y0, int x1, int y1, int depth=1.0f);
Matrix lookAt(Vec3f eye, Vec3f centre, Vec3f up);
Matrix formRightAngledProjection(float n, float f);
Matrix formProjection(float l, float r, float b, float t, float n, float f);

template <class t> struct Vec2 {
  union {
    struct {t u, v;};
    struct {t x, y;};
    t raw[2];
  };
  Vec2() : u(0), v(0) {}
  Vec2(t _u, t _v) : u(_u),v(_v) {}
  Vec2(const Vec2<t> &v) : x(v.x),y(v.y) {}
  Vec2(const Matrix& m) {
    assert((m.nrows() == 2) && (m.ncols()==1));
    x = m.get(0,0);
    y = m.get(1,0);
  }
  inline Vec2<t> operator +(const Vec2<t> &V) const { return Vec2<t>(u+V.u, v+V.v); }
  inline Vec2<t> operator -(const Vec2<t> &V) const { return Vec2<t>(u-V.u, v-V.v); }
  inline Vec2<t> operator *(float f)          const { return Vec2<t>(u*f, v*f); }
  inline bool operator==(const Vec2<t> &v) const { return x==v.x and y==v.y; }
  inline bool operator!=(const Vec2<t> &v) const { return !((*this)==v); }
  inline t& operator[](const int i) {return raw[i];}
  inline const t& operator[](const int i) const {return raw[i];}
  t norm2 () const { return x*x+y*y; }
  t norm () const { return std::sqrt(norm2()); }
  template <class > friend std::ostream& operator<<(std::ostream& s, const Vec2<t>& v);
};

template <class t> struct Vec3 {
  union {
    struct {t x, y, z, dummy1;};
    struct { t ivert, iuv, inorm, dummy2; };
    struct { t r, g, b, dummy3; };
    t raw[4];
  };
  Vec3() : x(0), y(0), z(0) {}
  Vec3(t _x, t _y, t _z) : x(_x),y(_y),z(_z) {}
  Vec3(const Vec3<t> &v) : x(v.x),y(v.y),z(v.z) {}
  Vec3(const Vec4<t> &v) : x(v.x),y(v.y),z(v.z) {}
  Vec3(const Matrix& m) {
    assert((m.nrows() == 3) && (m.ncols()==1));
    x = m.get(0,0);
    y = m.get(1,0);
    z = m.get(2,0);
  }
  inline Vec3<t> operator +(const Vec3<t> &v) const { return Vec3<t>(x+v.x, y+v.y, z+v.z); }
  inline Vec3<t> operator -(const Vec3<t> &v) const { return Vec3<t>(x-v.x, y-v.y, z-v.z); }
  inline Vec3<t> operator *(float f)          const { return Vec3<t>(x*f, y*f, z*f); }
  inline bool operator==(const Vec3<t> &v) const { return x==v.x and y==v.y and z==v.z; }
  inline bool operator!=(const Vec3<t> &v) const { return !((*this)==v); }
  inline t& operator[](const int i) {return raw[i];}
  inline const t& operator[](const int i) const {return raw[i];}
  inline t       dot(const Vec3<t> &v) const { return x*v.x + y*v.y + z*v.z; }
  inline Vec3<t> cross(const Vec3<t> &v) const { return Vec3<t>(y*v.z-z*v.y, z*v.x-x*v.z, x*v.y-y*v.x); }
  t norm2 () const { return x*x+y*y+z*z; }
  t norm () const { return std::sqrt(norm2()); }
  Vec3<t> & normalise(t l=1) { *this = (*this)*(l/norm()); return *this; }
  template <class > friend std::ostream& operator<<(std::ostream& s, const Vec3<t>& v);
};

template <class t> struct Vec4 {
  union {
    struct {t x, y, z, w;};
    struct { t ivert, iuv, inorm, dummy2; };
    struct { t r, g, b, a; };
    t raw[4];
  };
  Vec4() : x(0), y(0), z(0), w(0) {}
  Vec4(t _x, t _y, t _z, t _w=1) : x(_x),y(_y),z(_z),w(_w) {}
  Vec4(const Vec4<t> &v) : x(v.x),y(v.y),z(v.z),w(v.w) {}
  Vec4(const Vec3<t> &v, t _w=1) : x(v.x),y(v.y),z(v.z),w(_w) {}
  Vec4(const Matrix& m) {
    assert((m.nrows() == 4) && (m.ncols()==1));
    x = m.get(0,0);
    y = m.get(1,0);
    z = m.get(2,0);
    w = m.get(3,0);
  }
  inline Vec4<t> operator +(const Vec4<t> &v) const { return Vec4<t>(x+v.x, y+v.y, z+v.z); }
  inline Vec4<t> operator -(const Vec4<t> &v) const { return Vec4<t>(x-v.x, y-v.y, z-v.z); }
  inline Vec4<t> operator *(float f)          const { return Vec4<t>(x*f, y*f, z*f, 1); }
  inline bool operator==(const Vec4<t> &v) const { return x==v.x and y==v.y and z==v.z and w==v.w; }
  inline bool operator!=(const Vec4<t> &v) const { return not (*this==v); }
  inline t& operator[](const int i) {return raw[i];}
  inline const t& operator[](const int i) const {return raw[i];}
  inline t       dot(const Vec4<t> &v) const { return x*v.x + y*v.y + z*v.z; }
  inline Vec4<t> cross(const Vec4<t> &v) const { return Vec4<t>(y*v.z-z*v.y, z*v.x-x*v.z, x*v.y-y*v.x); }
  t norm2 () const { return x*x+y*y+z*z; }
  t norm () const { return std::sqrt(norm2()); }
  Vec4<t> & normalise(t l=1) { *this = (*this)*(l/norm()); return *this; }
  Vec4<t> & homogenise() {
    //assert(w!=0.f);
    *this = (*this)*(1/w);
    return *this;
  }
  template <class > friend std::ostream& operator<<(std::ostream& s, const Vec4<t>& v);
};


template <class t> std::ostream& operator<<(std::ostream& s, const Vec2<t>& v) {
  s << "(" << v.x << ", " << v.y << ")\n";
  return s;
}

template <class t> std::ostream& operator<<(std::ostream& s, const Vec3<t>& v) {
  s << "(" << v.x << ", " << v.y << ", " << v.z << ")\n";
  return s;
}

template <class t> std::ostream& operator<<(std::ostream& s, const Vec4<t>& v) {
  s << "(" << v.x << ", " << v.y << ", " << v.z << ", " << v.w << ")\n";
  return s;
}

Vec3f calcNormal(const Vec3f& v1, const Vec3f& v2, const Vec3f& v3);
Vec3f getBarycentricCoords(const Vec3f& A, const Vec3f& B, const Vec3f& C, const Vec3f& P);
Vec3f applyTransform(const Matrix& matrix, const Vec3f& v);
