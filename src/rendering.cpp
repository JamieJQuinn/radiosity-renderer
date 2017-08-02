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
  frame.flip_horizontally();
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
  frame.flip_horizontally();
  frame.write_tga_file(filename.c_str());
}

Vec3f getBarycentricCoords(const Vec3f& A, const Vec3f& B, const Vec3f& C, const Vec3f& P) {
  Vec3f u = Vec3f(B.x - A.x, C.x - A.x, A.x - P.x).cross(Vec3f(B.y - A.y, C.y - A.y, A.y - P.y));
  if (std::abs(u.z)<1) return Vec3f(-1,1,1); // triangle is degenerate, in this case return smth with negative coordinates
  return Vec3f(1.f-(u.x+u.y)/u.z, u.x/u.z, u.y/u.z);
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

Matrix viewportRelative(int x, int y, int w, int h, int depth) {
  Matrix m = Matrix::identity(4);
  m.set(0, 3, x+w/2.f);
  m.set(1, 3, y+h/2.f);
  m.set(2, 3, depth/2.f);

  m.set(0, 0, w/2.f);
  m.set(1, 1, h/2.f);
  m.set(2, 2, depth/2.f);
  return m;
}

Matrix lookAt(Vec3f eye, Vec3f centre, Vec3f up) {
  Vec3f z = (eye-centre).normalise();
  Vec3f x = up.cross(z).normalise();
  Vec3f y = z.cross(x).normalise();
  Matrix Minv = Matrix::identity();
  for (int i=0; i<3; i++) {
    Minv.set(0, i, x[i]);
    Minv.set(1, i, y[i]);
    Minv.set(2, i, z[i]);
  }
  return Minv;
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

Vec3f calcNormal(const Vec3f& v1, const Vec3f& v2, const Vec3f& v3) {
  // right handed normal
  return (v2-v1).cross(v3-v1).normalise();
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

template <class fillType, class zBufferType>
void clipAndRenderTriangle(Vec3f *screen_coords, Buffer<zBufferType>& zBuffer, Buffer<fillType> &buffer, const fillType& fillValue) {
  // Figure out intersection points
  float intersectPts[3];
  bool isIntersecting[3];
  bool intersectsAnywhere = false;
  for(int j=0; j<3; ++j) {
    intersectPts[j] = clipLineZ(screen_coords[j], screen_coords[(j+1)%3]);
    isIntersecting[j] = intersectPts[j] > 0.f and intersectPts[j] < 1.f;
    intersectsAnywhere = intersectsAnywhere or isIntersecting[j];
  }

  // Use intersections to deduce clipping strategy
  if( not intersectsAnywhere ) {
    // Full triangle is in front or behind
    if(isVertexInFront(screen_coords[0])) {
      // In front; render full triangle
      renderTriangle(screen_coords, zBuffer, buffer, fillValue);
    }
  } else {
    // figure out how clipping should be done
    for(int j=0; j<3; ++j) {
      if( not isIntersecting[j] ) {
        // Find non-intersecting edge pts
        Vec3f &edge1 = screen_coords[j];
        if(isVertexInFront(edge1)) {
          // Gotta split triangle into two
          Vec3f temp = screen_coords[(j+2)%3];
          screen_coords[(j+2)%3] = interpolate(
              screen_coords[(j+1)%3],
              screen_coords[(j+2)%3],
              intersectPts[(j+1)%3]);
          renderTriangle(screen_coords, zBuffer, buffer, fillValue);
          screen_coords[(j+1)%3] = screen_coords[(j+2)%3];
          screen_coords[(j+2)%3] = interpolate(
              temp,
              screen_coords[j],
              intersectPts[(j+2)%3]);
          renderTriangle(screen_coords, zBuffer, buffer, fillValue);
        } else {
          // Render one tri with intersection points
          screen_coords[j] = interpolate(
              screen_coords[(j-1)%3],
              screen_coords[j],
              intersectPts[j]);
          screen_coords[(j+1)%3] = interpolate(
              screen_coords[(j+1)%3],
              screen_coords[(j+2)%3],
              intersectPts[(j+1)%3]);
          renderTriangle(screen_coords, zBuffer, buffer, fillValue);
        }
      }
    }
  }
}

void transformFace(Vec3f* outScreenCoords, const Face& face, const Model& model, const Matrix& MVP) {
  for (int j=0; j<3; j++) {
    Vec3f v = model.vert(face[j].ivert);
    outScreenCoords[j] = m2v(MVP*v2m(v));
  }
}

void renderTestModelReflectivity(Buffer<TGAColor>& buffer, const Model& model, const Matrix& MVP) {
  Buffer<float> zBuffer(buffer.width, buffer.height, 0);
  for (int i=0; i<model.nfaces(); ++i) {
    Face face = model.face(i);
    TGAColor colour = getFaceColour(face, model);

    Vec3f screen_coords[3];
    transformFace(screen_coords, face, model, MVP);

    Vec3f n = calcNormal(screen_coords[0], screen_coords[1], screen_coords[2]);
    if(n.z>0.f) {
      //// ======= SIMPLE APPROACH
      bool isInFront = true;
      for(int j=0; j<3; ++j) {
        isInFront = isInFront and screen_coords[j].z < 1.f;
      }
      isInFront = true;
      if(isInFront) {
        renderTriangle(screen_coords, zBuffer, buffer, colour);
      }

      ////clipAndRenderTriangle(screen_coords, zBuffer, buffer, colour);
    }
  }
}
