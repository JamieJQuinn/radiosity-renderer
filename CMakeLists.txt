cmake_minimum_required(VERSION 3.7)
project("Radiosity in Computer Graphics")

include_directories(include)
add_executable(out src/main.cpp src/buffer.cpp)
