#include <iostream>

#include "precision.hpp"

using namespace std;

#define HEMICUBE_GRID_SIZE 10

class Patch {
};

class Vertex {
  public:
    real x, y, z;
}

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
  if (i < width and i >= 0) and (j < height and j >= 0) {
    buffer[j*width + i] = item;
  }
}

template <class T>
T Buffer<T>::get(int i, int j) {
  return buffer[j*width + i];
}

int main() {
  Buffer<int> buffer(HEMICUBE_GRID_SIZE, HEMICUBE_GRID_SIZE);
  buffer.set(1, 9, 10);
  cout << buffer.get(1, 9) << endl;
  return 0;
}
