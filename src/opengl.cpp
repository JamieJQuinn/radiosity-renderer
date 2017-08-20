#ifdef OPENGL
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GL/gl.h>
#include <glm/glm.hpp>

#include "opengl_helper.hpp"
#include "config.hpp"
#include "opengl.hpp"

OpenGLRenderer::OpenGLRenderer(const Model& model) {
  // Generic opengl init =========
  GLFWwindow* window;

  // Initialise GLFW
  if( !glfwInit() )
  {
    fprintf( stderr, "Failed to initialize GLFW\n" );
    getchar();
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
  }
  glfwMakeContextCurrent(window);

  // Initialize GLEW
  if (glewInit() != GLEW_OK) {
    fprintf(stderr, "Failed to initialize GLEW\n");
    getchar();
    glfwTerminate();
  }

  // End opengl ==========

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
  }

  // Render to our framebuffer
  glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);
  glViewport(0,0,HEMICUBE_GRID_SIZE,HEMICUBE_GRID_SIZE);

  // End texture stuff =========

  // Load shaders
  programID = LoadShaders("shaders/main.vert", "shaders/main.frag");
  MatrixID = glGetUniformLocation(programID, "MVP");

  initVertexBuffer(model);
  initColourBuffer(model);
  initIndexBuffer(model);

  // Initialise vertex VBO
  GLuint VertexArrayID;
  glGenVertexArrays(1, &VertexArrayID);
  glBindVertexArray(VertexArrayID);

  // Load vertex info from model
  nFaces = model.nfaces();
  nVerts = nFaces*3;

  glGenBuffers(1, &vertexBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*nVerts*3, vertex_buffer_data, GL_STATIC_DRAW);

  // Load colour info
  glGenBuffers(1, &colorBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*nVerts*3, colour_buffer_data, GL_STATIC_DRAW);

  // Load ID info
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
}

GLfloat * OpenGLRenderer::getVertexBuffer() {
  return vertex_buffer_data;
}

GLfloat * OpenGLRenderer::getColourBuffer() {
  return colour_buffer_data;
}

GLuint * OpenGLRenderer::getIndexBuffer() {
  return id_buffer_data;
}

void OpenGLRenderer::initVertexBuffer(const Model& model) {
  vertex_buffer_data = new GLfloat [model.nfaces()*3*3];
  int nFaces = model.nfaces();
  for(int i=0; i<nFaces; ++i) {
    Face f = model.face(i);
    for(int j=0; j<3; ++j) {
      Vec3f vertex = model.vert(f[j].ivert);
      for(int k=0; k<3; ++k) {
        vertex_buffer_data[9*i+3*j+k] = vertex[k];
      }
    }
  }
}

void OpenGLRenderer::initColourBuffer(const Model& model) {
  colour_buffer_data = new GLfloat [model.nfaces()*3*3];
  int nFaces = model.nfaces();
  for(int i=0; i<nFaces; ++i) {
    Vec3f colour = model.getFaceReflectivity(i);
    for(int j=0; j<3; ++j) {
      for(int k=0; k<3; ++k) {
        colour_buffer_data[9*i+3*j+k] = colour[k];
      }
    }
  }
}

void OpenGLRenderer::initIndexBuffer(const Model& model) {
  id_buffer_data = new GLuint [model.nfaces()*3];
  int nFaces = model.nfaces();
  for(int i=0; i<nFaces; ++i) {
    for(int j=0; j<3; ++j) {
      id_buffer_data[3*i+j] = (GLuint)(i+1);
    }
  }
}

void OpenGLRenderer::renderHemicube(Buffer<unsigned>& buffer, const glm::mat4& MVP) {
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

  glUseProgram(programID);

  glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

  // Vertices
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
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
  glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
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

  glGetTexImage(GL_TEXTURE_2D, 0, GL_RED_INTEGER, GL_UNSIGNED_INT, (GLuint*)buffer.getRow(0));
}
#endif
