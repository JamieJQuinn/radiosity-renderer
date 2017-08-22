#include <iostream>
#include <string>

#include "config.hpp"
#include "model.hpp"
#include "geometry.hpp"
#include "hemicube.hpp"
#include "rendering.hpp"
#include "colours.hpp"
#include "opengl_helper.hpp"
#include "opengl.hpp"

OpenGLRenderer * renderer = NULL;

int main(int argc, char* argv[]) {
  std::string modelObj(argv[1]);
  std::string modelMtl(argv[2]);

  int nPasses = atoi(argv[3]);

  std::cout << "obj: " << modelObj << std::endl;
  std::cout << "mtl: " << modelMtl << std::endl;
  std::cout << "Iterations: " << nPasses << std::endl;

  Model model(modelObj.c_str(), modelMtl.c_str());
  std::cerr << "Model setup." << std::endl;

#ifdef OPENGL
  renderer = new OpenGLRenderer(model);
#endif

  int gridSize = HEMICUBE_GRID_SIZE;

  std::cout << "Grid size: " << gridSize << std::endl;

  std::vector<Vec3f> radiosity(model.nfaces());

  std::cerr << "Form factor memory cost: " << sizeof(float)*model.nfaces()*model.nfaces()/(1024.f*1024.f) << " MB" << std::endl;
  Buffer<float> totalFormFactors(model.nfaces()+1, model.nfaces()+1, 0.f);
  std::cerr << "Calculating form factors" << std::endl;
  //calcFormFactorsWholeModel(model, totalFormFactors, gridSize);
  std::cerr << "Calculated form factors" << std::endl;

  //shootRadiosity(radiosity, model, gridSize, nPasses, totalFormFactors);
  gatherRadiosity(radiosity, model, gridSize, nPasses);

  Vec3f sum(0,0,0);
  for(int i=0; i<(int)radiosity.size(); ++i) {
    sum += radiosity[i];
  }
  std::cout << "Total radiosity: " << sum;

  std::vector<Vec3f> vertexRadiosity(model.nverts());
  radiosityFaceToVertex(vertexRadiosity, model, radiosity);

  renderVertexRadiosityToTexture(model, vertexRadiosity, 1200, "outputSmooth.tga");
  renderFaceRadiosityToTexture(model, radiosity, 1200, "output.tga");
}
