#include <iostream>
#include <cmath>

#include "rendering.hpp"
#include "tgaimage.hpp"
#include "geometry.hpp"
#include "buffer.hpp"
#include "model.hpp"
#include "colours.hpp"
#include "hemicube.hpp"

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

void renderModelReflectivity(Buffer<TGAColor>& buffer, const Model& model, const Matrix& MVP, const Vec3f& eye, float nearPlane) {
  Buffer<float> zBuffer(buffer.width, buffer.height, 0.f);
  for (int i=0; i<model.nfaces(); ++i) {
    Face face = model.face(i);
    TGAColor colour = model.getFaceColour(face);

    std::vector<Vec4f> pts = transformFace(face, model, MVP);

    Vec3f n = model.norm(i, 0);
    if( n.dot(model.centreOf(i)-eye) <= 0.f ) {
      clipAndRenderTriangle(pts, zBuffer, buffer, colour, nearPlane);
    }
  }
}

void renderModelRadiosity(Buffer<TGAColor>& buffer, const Model& model, const Matrix& MVP, const Vec3f& eye, float nearPlane, std::vector<Vec3f>& radiosity) {
  Buffer<float> zBuffer(buffer.width, buffer.height, 0.f);
  for (int i=0; i<model.nfaces(); ++i) {
    Face face = model.face(i);
    Vec3f rad = radiosity[i]*255.f;
    TGAColor colour = TGAColor(rad.r, rad.g, rad.b, 255);

    std::vector<Vec4f> pts = transformFace(face, model, MVP);

    Vec3f n = model.norm(i, 0);
    if( n.dot(model.centreOf(i)-eye) <= 0.f ) {
      clipAndRenderTriangle(pts, zBuffer, buffer, colour, nearPlane);
    }
  }
}

void renderVertexRadiosityToTexture(const Model& model, const std::vector<Vec3f>& radiosity, int size, std::string filename) {
  Buffer<TGAColor> buffer(size, size, black);

  for (int i=0; i<model.nfaces(); ++i) {
    Face face = model.face(i);
    std::vector<Vec3f> screen_coords(3);
    // Get uv coords of face in buffer space
    for (int j=0; j<3; j++) {
      screen_coords[j] = model.uv(i, j)*size;
    }

    Vec3f intensities[3] = {
      radiosity[face[0].ivert],
      radiosity[face[1].ivert],
      radiosity[face[2].ivert]
    };
    renderInterpolatedTriangle(screen_coords, buffer, intensities);
  }

  renderColourBuffer(buffer, filename);
}

void renderFaceRadiosityToTexture(const Model& model, const std::vector<Vec3f>& radiosity, int size, std::string filename) {
  Buffer<TGAColor> buffer(size, size, black);

  for (int i=0; i<model.nfaces(); ++i) {
    Face face = model.face(i);
    std::vector<Vec3f> screen_coords(3);
    // Get uv coords of face in buffer space
    for (int j=0; j<3; j++) {
      screen_coords[j] = model.uv(i, j)*size;
    }

    Vec3f rad = radiosity[i];
    TGAColor colour(rad.r*255, rad.g*255, rad.b*255, 255);
    renderTriangle(screen_coords, buffer, colour);
  }

  renderColourBuffer(buffer, filename);
}

void renderModelIds(Buffer<int>& buffer, const Model& model, const Matrix& MVP, const Vec3f& eye, float nearPlane) {
  Buffer<float> zBuffer(buffer.width, buffer.height, 0.f);
  for (int i=0; i<model.nfaces(); ++i) {
    Face face = model.face(i);
    std::vector<Vec4f> pts = transformFace(face, model, MVP);

    Vec3f n = model.norm(i, 0);
    if( n.dot(model.centreOf(i)-eye) <= 0.f ) {
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

void shootRadiosity(const Model& model, int gridSize, std::vector<Vec3f>& radiosity, std::vector<Vec3f>& radiosityToShoot, int faceIdx, const std::vector<float>& formFactors) {
  for(int j=0; j<model.nfaces(); ++j) {
    // Don't affect self
    if( j==faceIdx ) {
      continue;
    }
    float formFactor = formFactors[j+1];
    float areaThisPatch = model.area(faceIdx);
    float areaJthPatch = model.area(j);
    Vec3f reflectivity = model.getFaceReflectivity(j);

    Vec3f radiosityOut = radiosityToShoot[faceIdx].piecewise(reflectivity)
                         *(formFactor*areaThisPatch/areaJthPatch);

    radiosity[j] += radiosityOut;
    radiosityToShoot[j] += radiosityOut;
  }
  radiosityToShoot[faceIdx] = Vec3f(0,0,0);
}

void normaliseRadiosity(std::vector<Vec3f>& radiosity) {
  float max = 0.f;
  for(int i=0; i<(int)radiosity.size(); ++i) {
    for(int j=0; j<3; ++j) {
      max = radiosity[i][j] > max ? radiosity[i][j] : max;
    }
  }
  for(int i=0; i<(int)radiosity.size(); ++i) {
    radiosity[i] = radiosity[i] * (1.f/max);
  }
}

void calculateRadiosity(std::vector<Vec3f>& radiosity, const Model& model, int gridSize, int nPasses)  {
  // Setup radiosity
  std::vector<Vec3f> radiosityToShoot(model.nfaces());
  for(int i=0; i<model.nfaces(); ++i) {
    radiosityToShoot[i] = model.getFaceEmissivity(i);
    radiosity[i] = model.getFaceEmissivity(i);
  }

  std::vector<float> formFactors(model.nfaces()+1);
  std::vector<std::vector<float>> totalFormFactors(model.nfaces());
  for(int i=0; i<model.nfaces(); ++i) {
    calcFormFactorsFromModel(model, i, formFactors, gridSize);
    totalFormFactors[i] = formFactors;
  }

  for(int passes=0; passes<nPasses; ++passes) {
    for(int i=0; i<model.nfaces(); ++i) {
      shootRadiosity(model, gridSize, radiosity, radiosityToShoot, i, totalFormFactors[i]);
    }
  }

  normaliseRadiosity(radiosity);
}

void radiosityFaceToVertex(std::vector<Vec3f>& vertexRadiosity, const Model& model, const std::vector<Vec3f>& faceRadiosity) {
  std::vector<std::vector<Vec3f>> tempRadiosity(model.nverts());
  for(int i=0; i<model.nfaces(); ++i) {
    Face face = model.face(i);
    for(int j=0; j<face.size(); ++j) {
      tempRadiosity[face[j].ivert].push_back(faceRadiosity[i]);
    }
  }
  for(int i=0; i<(int)vertexRadiosity.size(); ++i) {
    vertexRadiosity[i] = Vec3f(0,0,0);
    for(int j=0; j<(int)tempRadiosity[i].size(); ++j) {
      vertexRadiosity[i] += tempRadiosity[i][j];
    }
    vertexRadiosity[i] = vertexRadiosity[i] * (1.f/tempRadiosity[i].size());
  }
}

void renderInterpolatedTriangle(const std::vector<Vec3f>& pts, Buffer<TGAColor> &buffer, const Vec3f intensities[3]) {
  // Create bounding box
  Vec2f bboxmin(buffer.width-1, buffer.height-1);
  Vec2f bboxmax(0, 0);
  Vec2f clamp(buffer.width-1, buffer.height-1);
  for (int i=0; i<3; i++) {
    for (int j=0; j<2; j++) {
      // clip against buffer sides
      bboxmin[j] = std::max(0.f, std::min(bboxmin[j], pts[i][j]));
      bboxmax[j] = std::min(clamp[j], std::max(bboxmax[j], pts[i][j]));
    }
  }

  // Check every pixel in bounding box
  Vec3f P;
  for (P.x=bboxmin.x; P.x<=bboxmax.x; ++P.x) {
    for (P.y=bboxmin.y; P.y<=bboxmax.y; ++P.y) {
      Vec3f bc_screen = getBarycentricCoords(pts[0], pts[1], pts[2], P);
      if (bc_screen.x<0 || bc_screen.y<0 || bc_screen.z<0) continue;
      P.z = 0;
      Vec3f intensity(0,0,0);
      for (int i=0; i<3; i++) {
        for(int j=0; j<3; ++j) {
          intensity[j] += intensities[i][j]*bc_screen[i];
        }
      }
      TGAColor colour(intensity.r*255, intensity.g*255, intensity.b*255, 255);
      buffer.set(int(P.x), int(P.y), colour);
    }
  }
}
