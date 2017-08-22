#pragma once
#include <GL/gl.h>
#include <glm/glm.hpp>

#include "model.hpp"
#include "buffer.hpp"

class OpenGLRenderer {
  public:
    OpenGLRenderer(const Model& model);
    ~OpenGLRenderer();
    void renderHemicube(Buffer<unsigned int>& buffer, const glm::mat4& MVP);
  private:
    GLuint programID;
    GLuint MatrixID;
    GLuint vertexBuffer;
    GLuint colorBuffer;
    GLuint idBuffer;
    GLfloat * vertex_buffer_data;
    GLfloat * colour_buffer_data;
    GLuint * id_buffer_data;
    int nFaces;
    int nVerts;

    GLfloat * getVertexBuffer();
    GLfloat * getColourBuffer();
    GLuint * getIndexBuffer();
    void initVertexBuffer(const Model& model);
    void initColourBuffer(const Model& model);
    void initIndexBuffer(const Model& model);
    OpenGLRenderer();
};

