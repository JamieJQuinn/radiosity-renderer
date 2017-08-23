#include <iostream>
#include <string>

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
  std::size_t pos = modelObj.find(".");
  std::string modelMtl = modelObj.substr(0, pos) + std::string(".mtl");

  std::cout << "obj: " << modelObj << std::endl;
  std::cout << "mtl: " << modelMtl << std::endl;

  Model model(modelObj.c_str(), modelMtl.c_str());
  std::cerr << "Model setup." << std::endl;
  std::cerr << "Num faces: " << model.nfaces() << std::endl;
  std::cerr << "Num verts: " << model.nverts() << std::endl;

#ifdef OPENGL
  std::cerr << "USING OPENGL" << std::endl;
  renderer = new OpenGLRenderer(model);
#endif

  int gridSize = HEMICUBE_GRID_SIZE;

  std::cout << "Grid size: " << gridSize << std::endl;

  std::vector<Vec3f> radiosity(model.nfaces());

#ifndef PROGRESSIVE
  std::cerr << "PRECALCULATING FORM FACTORS" << std::endl;
  std::cerr << "Form factor memory cost: " << sizeof(float)*model.nfaces()*model.nfaces()/(1024.f*1024.f) << " MB" << std::endl;
  Buffer<float> totalFormFactors(model.nfaces()+1, model.nfaces()+1, 0.f);
  std::cerr << "Calculating form factors" << std::endl;
  calcFormFactorsWholeModel(model, totalFormFactors, gridSize);
  std::cerr << "Calculated form factors" << std::endl;
#endif

#ifdef PROGRESSIVE
  std::cerr << "USING PROGRESSIVE REFINEMENT" << std::endl;

#ifdef SHOOTING
  std::cerr << "Shooting radiosity" << std::endl;
  shootRadiosity(radiosity, model, gridSize);
#endif
#ifdef GATHERING
  std::cerr << "gathering radiosity" << std::endl;
  gatherRadiosity(radiosity, model, gridSize);
#endif

#else

#ifdef SHOOTING
  std::cerr << "Shooting radiosity" << std::endl;
  shootRadiosity(radiosity, model, gridSize, totalFormFactors);
#endif
#ifdef GATHERING
  std::cerr << "gathering radiosity" << std::endl;
  gatherRadiosity(radiosity, model, gridSize, totalFormFactors);
#endif

#endif

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
