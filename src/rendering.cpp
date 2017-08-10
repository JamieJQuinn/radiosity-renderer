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

float clipLineZ(const Vec3f& v0, const Vec3f& v1, float nearPlane) {
  return (v0.z - nearPlane)/(v0.z - v1.z);
}

Vec3f interpolate(const Vec3f& v0, const Vec3f& v1, float t) {
  return v0 + (v1-v0)*t;
}

std::vector<Vec4f> transformFace(const Face& face, const Model& model, const Matrix& MVP) {
  std::vector<Vec4f> outScreenCoords(3);
  for (int j=0; j<3; j++) {
    Vec4f v(model.vert(face[j].ivert), 1);
    outScreenCoords[j] = MVP*v;
  }
  return outScreenCoords;
}

void renderModelReflectivity(Buffer<TGAColor>& buffer, const Model& model, const Matrix& MVP, const Vec3f& dir, float nearPlane) {
  Buffer<float> zBuffer(buffer.width, buffer.height, 0.f);
  for (int i=0; i<model.nfaces(); ++i) {
    Face face = model.face(i);
    TGAColor colour = model.getFaceColour(face);

    std::vector<Vec4f> pts = transformFace(face, model, MVP);

    Vec3f n = model.norm(i, 0);
    if( n.dot(dir) <= 0.f ) {
      clipAndRenderTriangle(pts, zBuffer, buffer, colour, nearPlane);
    }
  }
}

void renderModelIds(Buffer<int>& buffer, const Model& model, const Matrix& MVP, const Vec3f& dir, float nearPlane) {
  Buffer<float> zBuffer(buffer.width, buffer.height, 0.f);
  for (int i=0; i<model.nfaces(); ++i) {
    Face face = model.face(i);
    std::vector<Vec4f> pts = transformFace(face, model, MVP);

    Vec3f n = model.norm(i, 0);
    if( n.dot(dir) <= 0.f ) {
      clipAndRenderTriangle(pts, zBuffer, buffer, i+1, nearPlane);
    }
  }
}

void renderIdsToColour(const Buffer<int>& itemBuffer, const Model& model, std::string fileName) {
  Buffer<TGAColor> colourBuffer(itemBuffer.width, itemBuffer.height, black);
  for(int j=0; j<itemBuffer.height; ++j) {
    for(int i=0; i<itemBuffer.width; ++i) {
      int faceIdx = itemBuffer.get(i, j);
      if (faceIdx != 0) {
        colourBuffer.set(i, j, model.getFaceColour(faceIdx-1));
      }
    }
  }
  renderColourBuffer(colourBuffer, fileName);
}

int clipTriangle(std::vector<Vec4f>& pts, float nearPlane) {
  // Default no triangles to be rendered
  int nTrianglesReturned = 0;
  // Figure out intersection points
  float intersectPts[3];
  bool isIntersecting[3];
  bool intersectsAnywhere = false;
  for(int j=0; j<3; ++j) {
    intersectPts[j] = clipLineZ(pts[j], pts[(j+1)%3], nearPlane);
    isIntersecting[j] = intersectPts[j] > 0.f and intersectPts[j] < 1.f;
    intersectsAnywhere = intersectsAnywhere or isIntersecting[j];
  }

  // Use intersections to deduce clipping strategy
  if( not intersectsAnywhere ) {
    // Full triangle is in front or behind
    if(pts[0].z < nearPlane) {
      // In front; render full triangle
      nTrianglesReturned = 1;
    }
  } else {
    // figure out how clipping should be done
    for(int j=0; j<3; ++j) {
      if( not isIntersecting[j] ) {
        // Find non-intersecting edge pts
        int i1 = j;
        int i2 = (j+1)%3;
        int i3 = (j+2)%3;
        Vec4f v1 = pts[i1];
        Vec4f v2 = pts[i2];
        Vec4f v3 = pts[i3];
        if(v1.z < nearPlane) {
          // Gotta split triangle into two
          // Create new triangle
          pts.push_back(Vec4f(interpolate(
                v2, v3, intersectPts[i2]
                ), nearPlane));
          pts.push_back(Vec4f(interpolate(
                v3, v1, intersectPts[i3]
                ), nearPlane));
          pts.push_back(v1);
          // Fix triangle passed in
          pts[i3] = Vec4f(interpolate(v2, v3, intersectPts[i2]), nearPlane);
          nTrianglesReturned = 2;
        } else {
          // Render one tri with intersection points
          pts[i1] = Vec4f(interpolate(
              pts[i3],
              pts[i1],
              intersectPts[i3]), nearPlane);
          pts[i2] = Vec4f(interpolate(
              pts[i2],
              pts[i3],
              intersectPts[i2]), nearPlane);
          nTrianglesReturned = 1;
        }
        break;
      }
    }
  }
  return nTrianglesReturned;
}
