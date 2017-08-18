#include <iostream>
#include <string>

#ifdef OPENGL
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#endif

#include "model.hpp"
#include "geometry.hpp"
#include "hemicube.hpp"
#include "rendering.hpp"
#include "colours.hpp"

void mainCPU(int argc, char* argv[]) {
  std::string modelObj(argv[1]);
  std::string modelMtl(argv[2]);

  int nPasses = atoi(argv[3]);

  std::cout << "obj: " << modelObj << std::endl;
  std::cout << "mtl: " << modelMtl << std::endl;
  std::cout << "Iterations: " << nPasses << std::endl;

  Model model(modelObj.c_str(), modelMtl.c_str());
  std::cerr << "Model setup." << std::endl;
  int gridSize = 256;

  std::cout << "Grid size: " << gridSize << std::endl;

  std::vector<Vec3f> radiosity(model.nfaces());
  calculateRadiosity(radiosity, model, gridSize, nPasses);

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

#ifdef OPENGL
int mainOpenGL(int argc, char* argv[]) {
  GLFWwindow* window;

  // Initialise GLFW
  if( !glfwInit() )
  {
    fprintf( stderr, "Failed to initialize GLFW\n" );
    getchar();
    return -1;
  }

  glfwWindowHint(GLFW_SAMPLES, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // Open a window and create its OpenGL context
  window = glfwCreateWindow( 1024, 768, "Tutorial 01", NULL, NULL);
  if( window == NULL ){
    fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
    getchar();
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);

  // Initialize GLEW
  if (glewInit() != GLEW_OK) {
    fprintf(stderr, "Failed to initialize GLEW\n");
    getchar();
    glfwTerminate();
    return -1;
  }

  // Ensure we can capture the escape key being pressed below
  glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

  // Dark blue background
  glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

  do{
    // Clear the screen. It's not mentioned before Tutorial 02, but it can cause flickering, so it's there nonetheless.
    glClear( GL_COLOR_BUFFER_BIT );

    // Draw nothing, see you in tutorial 2 !

    // Swap buffers
    glfwSwapBuffers(window);
    glfwPollEvents();

  } // Check if the ESC key was pressed or the window was closed
  while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
       glfwWindowShouldClose(window) == 0 );

  // Close OpenGL window and terminate GLFW
  glfwTerminate();

  return 0;
}
#endif

int main(int argc, char* argv[]) {
#ifndef OPENGL
  mainCPU(argc, argv);
#else
  mainOpenGL(argc, argv);
#endif

  return 0;
}
