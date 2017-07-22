#include "catch.hpp"

#include <vector>
#include <fstream>

#include "geometry.hpp"
#include "model.hpp"
#include "tgaimage.hpp"

TEST_CASE("Output radiosity text file", "[output]") {
  Model model("test/simple_box.obj");
  std::vector<TGAColor> radiosity;
  int nFaces = model.nfaces();

  // generate fake radiosity
  for(int i=0; i<nFaces; ++i) {
    float intensity = float(i)/nFaces*255;
    radiosity.push_back(TGAColor(intensity, intensity, intensity, 255));
  }

  // print out radiosity per face
  std::ofstream testFile("test/simple_box_radiosity.txt");
  for(int i=0; i<nFaces; ++i) {
    testFile << "r " << (int)radiosity[i].r << "\\" << (int)radiosity[i].g << "\\" << (int)radiosity[i].b << std::endl;
  }
  testFile.close();
}
