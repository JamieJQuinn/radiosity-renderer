#include <iostream>
#include <string>

#ifdef OPENGL
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#endif

#define HEMICUBE_GRID_SIZE 512

#include "model.hpp"
#include "geometry.hpp"
#include "hemicube.hpp"
#include "rendering.hpp"
#include "colours.hpp"
#include "opengl_helper.hpp"

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
  window = glfwCreateWindow( HEMICUBE_GRID_SIZE, HEMICUBE_GRID_SIZE, "Radiosity", NULL, NULL);
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

  // TEXTURE STUFF ======

  // Setup texture output
  GLuint FramebufferName = 0;
  glGenFramebuffers(1, &FramebufferName);
  glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);

  // The texture we're going to render to
  GLuint renderedTexture;
  glGenTextures(1, &renderedTexture);

  // "Bind" the newly created texture : all future texture functions will modify this texture
  glBindTexture(GL_TEXTURE_2D, renderedTexture);

  // Give an empty image to OpenGL ( the last "0" )
  glTexImage2D(GL_TEXTURE_2D, 0, GL_R32UI, HEMICUBE_GRID_SIZE, HEMICUBE_GRID_SIZE, 0, GL_RED_INTEGER, GL_UNSIGNED_INT, 0);

  GLuint depthrenderbuffer;
  glGenRenderbuffers(1, &depthrenderbuffer);
  glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, HEMICUBE_GRID_SIZE, HEMICUBE_GRID_SIZE);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);

  // Set "renderedTexture" as our colour attachement #0
  glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderedTexture, 0);

  // Set the list of draw buffers.
  GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
  glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers

  if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
    std::cerr << "Couldn't load framebuffer" << std::endl;
    return -1;
  }

  // Render to our framebuffer
  glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);
  glViewport(0,0,HEMICUBE_GRID_SIZE,HEMICUBE_GRID_SIZE);

  // END TEXTURE STUFF ========

  // Load shaders
  GLuint programID = LoadShaders("shaders/main.vert", "shaders/main.frag");

  // ======== TEST STUFF
  Model model("test/scene.obj", "test/scene.mtl");
  int faceIdx = 0;
  Vec3f eye = model.centreOf(faceIdx);
  Vec3f dir = model.norm(faceIdx, 0);
  Vec3f up = getUp(dir);
  glm::vec3 glmEye = glmVec3FromVec3f(eye);
  glm::vec3 glmCentre = glmVec3FromVec3f(eye + dir);
  //glm::vec3 glmEye = glmVec3FromVec3f(Vec3f(2,3,4));
  //glm::vec3 glmCentre = glmVec3FromVec3f(Vec3f(0,0,0));
  glm::vec3 glmUp = glmVec3FromVec3f(up);
  // ========================

  // create MVP
  glm::mat4 CameraMatrix = glm::lookAt(glmEye,glmCentre,glmUp);
  glm::mat4 Projection = glm::perspective(glm::radians(90.0f), 1.f, 0.1f, 100.0f);
  glm::mat4 MVP = Projection*CameraMatrix;

  GLuint MatrixID = glGetUniformLocation(programID, "MVP");

  // Initialise vertex VBO
  GLuint VertexArrayID;
  glGenVertexArrays(1, &VertexArrayID);
  glBindVertexArray(VertexArrayID);

  // Load vertex info from model
  const int nFaces = model.nfaces();
  const int nVerts = nFaces*3;

  GLfloat * g_vertex_buffer_data = new GLfloat [nVerts*3];
  for(int i=0; i<nFaces; ++i) {
    Face f = model.face(i);
    for(int j=0; j<3; ++j) {
      Vec3f vertex = model.vert(f[j].ivert);
      for(int k=0; k<3; ++k) {
        g_vertex_buffer_data[9*i+3*j+k] = vertex[k];
      }
    }
  }

  GLuint vertexbuffer;
  glGenBuffers(1, &vertexbuffer);
  glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*nVerts*3, g_vertex_buffer_data, GL_STATIC_DRAW);

  // Load colour info
  GLfloat * g_colour_buffer_data = new GLfloat [nVerts*3];
  for(int i=0; i<nFaces; ++i) {
    Vec3f colour = model.getFaceReflectivity(i);
    for(int j=0; j<3; ++j) {
      for(int k=0; k<3; ++k) {
        g_colour_buffer_data[9*i+3*j+k] = colour[k];
      }
    }
  }

  GLuint colorbuffer;
  glGenBuffers(1, &colorbuffer);
  glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*nVerts*3, g_colour_buffer_data, GL_STATIC_DRAW);

  // Load ID info
  GLuint *id_buffer_data = new GLuint [nFaces*3];
  for(int i=0; i<nFaces; ++i) {
    for(int j=0; j<3; ++j) {
      id_buffer_data[3*i+j] = (GLuint)(i+1);
    }
  }

  GLuint idBuffer;
  glGenBuffers(1, &idBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, idBuffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLuint)*nFaces*3, id_buffer_data, GL_STATIC_DRAW);

  // Enable z-buffer
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  // enable back face culling
  glEnable(GL_CULL_FACE);

  // Dark blue background
  glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

  glUseProgram(programID);

  glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

  // Vertices
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
  glVertexAttribPointer(
     0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
     3,                  // size
     GL_FLOAT,           // type
     GL_FALSE,           // normalized?
     0,                  // stride
     (void*)0            // array buffer offset
  );

  // Colours
  glEnableVertexAttribArray(1);
  glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
  glVertexAttribPointer(
    1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
    3,                                // size
    GL_FLOAT,                         // type
    GL_FALSE,                         // normalized?
    0,                                // stride
    (void*)0                          // array buffer offset
  );

  glEnableVertexAttribArray(2);
  glBindBuffer(GL_ARRAY_BUFFER, idBuffer);
  glVertexAttribIPointer(
    2,                                // attribute. No particular reason for 1, but must match the layout in the shader.
    1,                                // size
    GL_UNSIGNED_INT,                         // type
    0,                                // stride
    0                          // array buffer offset
  );

  // Draw
  glDrawArrays(GL_TRIANGLES, 0, nVerts);
  glDisableVertexAttribArray(0);
  glDisableVertexAttribArray(1);
  glDisableVertexAttribArray(2);

  GLuint* textureOut = new GLuint[HEMICUBE_GRID_SIZE*HEMICUBE_GRID_SIZE];
  glGetTexImage(GL_TEXTURE_2D, 0, GL_RED_INTEGER, GL_UNSIGNED_INT, textureOut);
  renderIdsToColour(textureOut, HEMICUBE_GRID_SIZE, model, "test/gl_test_out.tga");

  //GLubyte* textureOut = new GLubyte[HEMICUBE_GRID_SIZE*HEMICUBE_GRID_SIZE*3];
  //glReadPixels(0,0,HEMICUBE_GRID_SIZE,HEMICUBE_GRID_SIZE,GL_RGB,GL_UNSIGNED_BYTE, textureOut);

  //renderColourBuffer(textureOut, HEMICUBE_GRID_SIZE, "test/gl_test_out.tga");

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
