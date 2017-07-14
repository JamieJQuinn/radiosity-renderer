#pragma once

template <class T>
class Buffer {
  public:
    void set(int i, int j, T);
    T get(int i, int j);
    Buffer(int _width, int _height);
    ~Buffer();
  private:
    int width; // in pixels
    int height; // in pixels
    T* buffer;
    Buffer();
};
