#include <iostream>
#include <cmath>

#include "rendering.hpp"
#include "tgaimage.hpp"
#include "geometry.hpp"
#include "buffer.hpp"
#include "model.hpp"
#include "colours.hpp"

void renderColourBuffer(const Buffer<TGAColor>& buffer, TGAImage& image) {
  for(int j=0; j<buffer.height; ++j) {
    for(int i=0; i<buffer.width; ++i) {
      image.set(i, j, buffer.get(i, j));
    }
  }
}

void renderColourBuffer(const Buffer<TGAColor>& buffer, std::string filename) {
  TGAImage frame(buffer.width, buffer.height, TGAImage::RGB);
  renderColourBuffer(buffer, frame);
  frame.flip_vertically();
  frame.write_tga_file(filename.c_str());
}

void renderZBuffer(const Buffer<float>& zBuffer, TGAImage& image) {
  float maxZ = zBuffer.max();
  for(int j=0; j<zBuffer.height; ++j) {
    for(int i=0; i<zBuffer.width; ++i) {
      float zVal = 255*zBuffer.get(i, j)/maxZ;
      TGAColor grey = TGAColor(zVal, zVal, zVal, 255);
      image.set(i, j, grey);
    }
  }
}

void renderZBuffer(const Buffer<float>& zBuffer, std::string filename) {
  TGAImage frame(zBuffer.width, zBuffer.height, TGAImage::RGB);
  renderZBuffer(zBuffer, frame);
  frame.flip_vertically();
  frame.write_tga_file(filename.c_str());
}

void renderWireFrame(const Model& model, Buffer<TGAColor>& buffer, const Matrix& MVP) {
  for (int i=0; i<model.nfaces(); i++) {
    Face face = model.face(i);
    std::swap(face[0], face[1]);
    int nVerts = face.size();
    for (int j=0; j<nVerts; j++) {
      Vec3f v0 = m2v(MVP*v2m(model.vert(face[j].ivert)));
      Vec3f v1 = m2v(MVP*v2m(model.vert(face[(j+1)%nVerts].ivert)));
      renderLine(v0.x, v0.y, v1.x, v1.y, buffer, white);
    }
  }
}

void calcFormFactorPerCell(const int sideLengthInPixels, Buffer<float>& topFace, Buffer<float>& sideFace) {
  assert(sideLengthInPixels%2==0); // Need to half for side face

  float pixelLength = 2.f/sideLengthInPixels;
  float dA = pixelLength*pixelLength;

  float initialX = -1.f + pixelLength/2.f;
  float initialY = -1.f + pixelLength/2.f;
  for(int j=0; j<sideLengthInPixels; ++j) {
    for(int i=0; i<sideLengthInPixels; ++i) {
      float x = initialX + pixelLength*i;
      float y = initialY + pixelLength*j;
      float r = x*x + y*y + 1.f;
      float factor = dA/(r*r*M_PI);
      topFace.set(i, j, factor);
    }
  }

  float initialZ = pixelLength/2.f;
  for(int j=0; j<sideLengthInPixels/2; ++j) {
    for(int i=0; i<sideLengthInPixels; ++i) {
      float z = initialZ + pixelLength*j;
      float y = initialY + pixelLength*i;
      float r = z*z + y*y + 1.f;
      float factor = z*dA/(r*r*M_PI);
      sideFace.set(i, j, factor);
    }
  }
}

void calcFormFactorsFromBuffer(const Buffer<int>& itemBuffer, const Buffer<float>& factorsPerCell, std::vector<float>& formFactors) {
  for(int j=0; j<itemBuffer.height; ++j) {
    for(int i=0; i<itemBuffer.width; ++i) {
      int idx = itemBuffer.get(i, j);
      formFactors[idx] += factorsPerCell.get(i, j);
    }
  }
}

void renderModel(Buffer<int>& buffer, const Model& model, const Matrix& MVP) {
  Buffer<float> zBuffer(buffer.width, buffer.height, -1);
  for (int i=0; i<model.nfaces(); ++i) {
    Face face = model.face(i);
    Vec3f screen_coords[3];
    for (int j=0; j<3; j++) {
      Vec3f v = model.vert(face[j].ivert);
      screen_coords[j] = m2v(MVP*v2m(v));
    }
    Vec3f n = calcNormal(screen_coords[0], screen_coords[1], screen_coords[2]);
    bool isInFront = true;
    for(int j=0; j<3; ++j) {
      isInFront = isInFront and screen_coords[j].z > 0.f;
    }
    if (n.z<0 and isInFront) {
      // Render to ID itembuffer
      renderTriangle(screen_coords, zBuffer, buffer, i+1);
    }
  }
}

Vec3f clipAgainstZPlane(const Vec3f* screen_coords)  {
  Vec3f t;
  for(int i=0; i<3; ++i) {
    t[i] = screen_coords[i].z - 1.f/(screen_coords[i].z - screen_coords[(i+1)%3].z);
  }
  return t;
}

float clipLineZ(const Vec3f& v0, const Vec3f& v1) {
  return (v0.z - 1.0f)/(v0.z - v1.z);
}

bool isVertexInFront(const Vec3f& v) {
  return v.z < 1.f;
}

Vec3f interpolate(const Vec3f& v0, const Vec3f& v1, float t) {
  return v0 + (v1-v0)*t;
}

TGAColor getFaceColour(const Face& face, const Model& model) {
  Vec3f matColour = model.material(face.matIdx).reflectivity*255;
  return TGAColor(matColour.r, matColour.g, matColour.b, 255);
}

void transformFace(Vec3f* outScreenCoords, const Face& face, const Model& model, const Matrix& MVP) {
  for (int j=0; j<3; j++) {
    Vec3f v = model.vert(face[j].ivert);
    outScreenCoords[j] = m2v(MVP*v2m(v));
  }
}

void renderTestModelReflectivity(Buffer<TGAColor>& buffer, const Model& model, const Matrix& MVP) {
  Buffer<float> zBuffer(buffer.width, buffer.height, 0.f);
  for (int i=0; i<model.nfaces(); ++i) {
    Face face = model.face(i);
    TGAColor colour = getFaceColour(face, model);

    Vec3f screen_coords[3];
    transformFace(screen_coords, face, model, MVP);

    Vec3f n = calcNormal(screen_coords[0], screen_coords[1], screen_coords[2]);
    if(n.z>0.f) {
      clipAndRenderTriangle(screen_coords, zBuffer, buffer, colour);
    }
  }
}
