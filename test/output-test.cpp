#include "catch.hpp"

#include <vector>
#include <fstream>

#include "geometry.hpp"
#include "model.hpp"
#include "tgaimage.hpp"
#include "colours.hpp"
#include "buffer.hpp"
#include "rendering.hpp"

void outputRadiosity(const std::vector<TGAColor>& radiosity, const std::string& filename) {
  std::ofstream file(filename.c_str());
  int size = radiosity.size();
  for(int i=0; i<size; ++i) {
    file << "r " << (int)radiosity[i].r << "\\" << (int)radiosity[i].g << "\\" << (int)radiosity[i].b << std::endl;
  }
  file.close();
}

TEST_CASE("Output radiosity text file", "[output]") {
  Model model("test/simple_box.obj", "test/simple_box.mtl");
  std::vector<TGAColor> radiosity;
  int nFaces = model.nfaces();

  // generate fake radiosity
  for(int i=0; i<nFaces; ++i) {
    Face face = model.face(i);
    Vec3f matColour = model.material(face.matIdx).reflectivity*255;
    radiosity.push_back(TGAColor(matColour.r, matColour.g, matColour.b, 255));
  }

  // print out radiosity per face
  outputRadiosity(radiosity, "test/simple_box_radiosity.txt");
}

TEST_CASE("Output texture based on input uv coords", "[output]") {
  Model model("test/simple_box.obj", "test/simple_box.mtl");
  int size = 150;
  Buffer<TGAColor> buffer(size, size, black);

  for (int i=0; i<model.nfaces(); ++i) {
    Face face = model.face(i);
    std::vector<Vec3f> screen_coords(3);
    // Get uv coords of face in buffer space
    for (int j=0; j<3; j++) {
      screen_coords[j] = model.uv(i, j)*size;
    }

    Vec3f matColour = model.material(face.matIdx).reflectivity*255;
    TGAColor colour = TGAColor(matColour.r, matColour.g, matColour.b, 255);
    renderTriangle(screen_coords, buffer, colour);
  }

  renderColourBuffer(buffer, "test/simple_box_texture.tga");
}
