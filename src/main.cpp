#include <iostream>
#include <string>

#include "model.hpp"
#include "geometry.hpp"
#include "hemicube.hpp"
#include "rendering.hpp"
#include "colours.hpp"

int main(int argc, char* argv[]) {
  std::string modelObj(argv[1]);
  std::string modelMtl(argv[2]);

  int nPasses = atoi(argv[3]);

  Model model(modelObj.c_str(), modelMtl.c_str());
  int gridSize = 128;

  std::vector<Vec3f> radiosity(model.nfaces());
  calculateRadiosity(radiosity, model, gridSize, nPasses);

  Vec3f sum(0,0,0);
  for(int i=0; i<(int)radiosity.size(); ++i) {
    sum += radiosity[i];
  }
  std::cout << sum;

  // Render
  Vec3f eye(-1.5, 0, 0);
  Vec3f dir(1, 0, 0);
  Vec3f up(0, 0, 1);
  int size = 800;
  float nearPlane = 0.05f;
  Matrix MVP = formHemicubeMVP(eye, dir, up);

  Buffer<TGAColor> buffer(size, size, black);
  renderModelRadiosity(buffer, model, MVP, eye, nearPlane, radiosity);

  renderColourBuffer(buffer, "output.tga");

  return 0;
}
