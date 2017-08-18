#version 330 core

in vec3 fragmentColor;
flat in uint fragmentID;

//layout(location=0) out vec3 color;
layout(location=0) out uint id;

void main(){
  id = fragmentID;
}
