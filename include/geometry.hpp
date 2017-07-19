#pragma once

#include <cmath>
#include <ostream>

template <class t> struct Vec2 {
  union {
    struct {t u, v;};
    struct {t x, y;};
    t raw[2];
  };
  Vec2() : u(0), v(0) {}
  Vec2(t _u, t _v) : u(_u),v(_v) {}
  Vec2(const Vec2<t> &v) : x(v.x),y(v.y) {}
  inline Vec2<t> operator +(const Vec2<t> &V) const { return Vec2<t>(u+V.u, v+V.v); }
  inline Vec2<t> operator -(const Vec2<t> &V) const { return Vec2<t>(u-V.u, v-V.v); }
  inline Vec2<t> operator *(float f)          const { return Vec2<t>(u*f, v*f); }
  inline bool operator==(const Vec2<t> &v) const { return x==v.x and y==v.y; }
  inline bool operator!=(const Vec2<t> &v) const { return !((*this)==v); }
  inline t& operator[](const int i) {return raw[i];}
  inline const t& operator[](const int i) const {return raw[i];}
  t norm2 () const { return x*x+y*y; }
  t norm () const { return std::sqrt(norm2()); }
  template <class > friend std::ostream& operator<<(std::ostream& s, Vec2<t>& v);
};

template <class t> struct Vec3 {
  union {
    struct {t x, y, z, dummy1;};
    struct { t ivert, iuv, inorm, dummy2; };
    t raw[4];
  };
  Vec3() : x(0), y(0), z(0) {}
  Vec3(t _x, t _y, t _z) : x(_x),y(_y),z(_z) {}
  Vec3(const Vec3<t> &v) : x(v.x),y(v.y),z(v.z) {}
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
  template <class > friend std::ostream& operator<<(std::ostream& s, Vec3<t>& v);
};

typedef Vec2<float> Vec2f;
typedef Vec2<int>   Vec2i;
typedef Vec3<float> Vec3f;
typedef Vec3<int>   Vec3i;

template <class t> std::ostream& operator<<(std::ostream& s, Vec2<t>& v) {
  s << "(" << v.x << ", " << v.y << ")\n";
  return s;
}

template <class t> std::ostream& operator<<(std::ostream& s, Vec3<t>& v) {
  s << "(" << v.x << ", " << v.y << ", " << v.z << ")\n";
  return s;
}