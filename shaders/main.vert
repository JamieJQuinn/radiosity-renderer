#version 330 core

layout(location=0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec3 vertexColor;
layout(location = 2) in uint id;

flat out uint fragmentID;
out vec3 fragmentColor;

uniform mat4 MVP;

void main() {
  gl_Position = MVP*vec4(vertexPosition_modelspace, 1.0);
  fragmentColor = vertexColor;
  fragmentID = id;
}
