#include "buffer.hpp"

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
  // TODO Is bounds check necessary?
  if ((i < width and i >= 0) and (j < height and j >= 0)) {
    buffer[j*width + i] = item;
  }
}

template <class T>
T Buffer<T>::get(int i, int j) {
  return buffer[j*width + i];
}

