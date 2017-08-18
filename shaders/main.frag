#version 330 core

in vec3 fragmentColor;
flat in uint fragmentID;

layout(location=0) out vec3 color;

void main(){
  //float colour = float(fragmentID)/768.0f;
  //color = vec3(colour, colour, colour);
  if(fragmentID > uint(400)) {
    color = vec3(0,0,1);
  } else {
    color = vec3(0,1,0);
  }
}
