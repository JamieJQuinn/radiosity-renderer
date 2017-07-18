#pragma once
#include "vertex.hpp"
#include <cassert>

template <class T>
class Buffer {
  public:
    void set(int i, int j, const T&);
    T get(int i, int j) const;
    Buffer(int _width, int _height);
    Buffer(int _width, int _height, T initial);
    void setup(int _width, int _height);
    ~Buffer();
    void fillAll(T fillData);
  protected:
    int width, height; // in pixels
    T* buffer;
    Buffer();
    Buffer(const Buffer&);
};

template <class T>
Buffer<T>::Buffer(int _width, int _height) {
  setup(_width, _height);
}

template <class T>
void Buffer<T>::setup(int _width, int _height) {
  buffer = new T[_width*_height];
  width = _width;
  height = _height;
}

template <class T>
Buffer<T>::Buffer(int _width, int _height, T initial) {
  setup(_width, _height);
  fillAll(initial);
}

template <class T>
Buffer<T>::~Buffer() {
  delete [] buffer;
}

template <class T>
void Buffer<T>::set(int i, int j, const T& item) {
  assert((i < width and i >= 0) and (j < height and j >= 0));
  buffer[j*width + i] = item;
}

template <class T>
T Buffer<T>::get(int i, int j) const {
  return buffer[j*width + i];
}

template <class T>
void Buffer<T>::fillAll(T fillData) {
  for(int i=0; i<width; ++i) {
    for(int j=0; j<height; ++j) {
      set(i, j, fillData);
    }
  }
}
