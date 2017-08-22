#pragma once
#include <cassert>
#include <iostream>

template <class T>
class Buffer {
  public:
    void set(int i, int j, const T&);
    const T get(int i, int j) const;
    T& get(int i, int j);
    T* getRow(int j);
    Buffer(int _width, int _height);
    Buffer(int _width, int _height, T initial);
    void setup(int _width, int _height);
    T max() const;
    T sum() const;
    ~Buffer();
    void fillAll(T fillData);
    int width, height; // in pixels
    template <class t> friend std::ostream& operator<<(std::ostream& s, Buffer<t>& b);
  protected:
    T* buffer;
    Buffer();
    Buffer(const Buffer&);
};

template <class T>
T* Buffer<T>::getRow(int j) {
  return buffer + j*width;
}

template <class T>
T Buffer<T>::max() const {
  T max = buffer[0];
  for(int i=1; i<width*height; ++i) {
    if(buffer[i] > max) {
      max = buffer[i];
    }
  }
  return max;
}

template <class T>
T Buffer<T>::sum() const {
  T sum = buffer[0];
  for(int i=1; i<width*height; ++i) {
    sum += buffer[i];
  }
  return sum;
}

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
  if((i < width and i >= 0) and (j < height and j >= 0)) {
    buffer[j*width + i] = item;
  }
}

template <class T>
T& Buffer<T>::get(int i, int j) {
  return buffer[j*width + i];
}

template <class T>
const T Buffer<T>::get(int i, int j) const {
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

template <class t> std::ostream& operator<<(std::ostream& s, Buffer<t>& b) {
  for(int j=0; j<b.height; ++j) {
    for(int i=0; i<b.width; ++i) {
      s << b.get(i, j) << ",";
    }
    s << std::endl;
  }
  return s;
}
