#pragma once
#include "vertex.hpp"
#include <cassert>

template <class T>
class Buffer {
  public:
    void set(int i, int j, T);
    T get(int i, int j);
    Buffer(int _width, int _height);
    ~Buffer();
    void fillTriangle(Vertex v1, Vertex v2, Vertex v3, T fillData);
    void fillLine(int x1, int x2, int y, T fillData);
    void fillAll(T fillData);
  protected:
    int width; // in pixels
    int height; // in pixels
    T* buffer;
    Buffer();

    void fillBottomFlatTriangle(Vertex v1, Vertex v2, Vertex v3, T fillData);
    void fillTopFlatTriangle(Vertex v1, Vertex v2, Vertex v3, T fillData);
};

template <class T>
Buffer<T>::Buffer(int _width, int _height) {
  buffer = new T[_width*_height];
  width = _width;
  height = _height;
}

template <class T>
Buffer<T>::~Buffer() {
  delete [] buffer;
}

template <class T>
void Buffer<T>::set(int i, int j, T item) {
  assert((i < width and i >= 0) and (j < height and j >= 0));
  buffer[j*width + i] = item;
}

template <class T>
T Buffer<T>::get(int i, int j) {
  return buffer[j*width + i];
}

template <class T>
void Buffer<T>::fillLine(int x1, int x2, int y, T fillData) {
  for(int x=x1; x<=x2; ++x) {
    this->set(x, y, fillData);
  }
}

template <class T>
void Buffer<T>::fillTriangle(Vertex v1, Vertex v2, Vertex v3, T fillData) {
  // Assume vertices in clockwise order & sorted in y dir (i.e. v1.y <= v2.y <= v3.y)
  assert(v1.y <= v2.y);
  assert(v2.y <= v3.y);

  /* check for trivial case of bottom-flat triangle */
  if (v2.y == v3.y)
  {
    fillBottomFlatTriangle(v1, v2, v3, fillData);
  }
  /* check for trivial case of top-flat triangle */
  else if (v1.y == v2.y)
  {
    fillTopFlatTriangle(v1, v2, v3, fillData);
  }
  else
  {
    /* general case - split the triangle in a topflat and bottom-flat one */
    Vertex v4((int)(v1.x + ((real)(v2.y - v1.y) / (real)(v3.y - v1.y)) * (v3.x - v1.x)), v2.y);
    if(v4.x < v2.x) {
      fillBottomFlatTriangle(v1, v2, v4, fillData);
      fillTopFlatTriangle(v4, v2, v3, fillData);
    } else {
      fillBottomFlatTriangle(v1, v4, v2, fillData);
      fillTopFlatTriangle(v2, v4, v3, fillData);
    }
  }
}

template <class T>
void Buffer<T>::fillBottomFlatTriangle(Vertex v1, Vertex v2, Vertex v3, T fillData) {
  assert(v2.y == v3.y);
  assert(v1.y < v2.y);

  real invslope1 = (v2.x - v1.x) / (v2.y - v1.y);
  real invslope2 = (v3.x - v1.x) / (v3.y - v1.y);

  real curx1 = v1.x;
  real curx2 = v1.x;

  for (int scanlineY = v1.y; scanlineY <= v2.y; scanlineY++)
  {
    fillLine((int)curx2, (int)curx1, scanlineY, fillData);
    curx1 += invslope1;
    curx2 += invslope2;
  }
}

template <class T>
void Buffer<T>::fillTopFlatTriangle(Vertex v1, Vertex v2, Vertex v3, T fillData)
{
  assert(v1.y == v2.y);
  assert(v1.y < v3.y);

  real invslope1 = (v3.x - v1.x) / (v3.y - v1.y);
  real invslope2 = (v3.x - v2.x) / (v3.y - v2.y);

  real curx1 = v3.x;
  real curx2 = v3.x;

  for (int scanlineY = v3.y; scanlineY > v1.y; scanlineY--)
  {
    fillLine((int)curx1, (int)curx2, scanlineY, fillData);
    curx1 -= invslope1;
    curx2 -= invslope2;
  }
}

template <class T>
void Buffer<T>::fillAll(T fillData) {
  for(int i=0; i<width; ++i) {
    for(int j=0; j<height; ++j) {
      set(i, j, fillData);
    }
  }
}
