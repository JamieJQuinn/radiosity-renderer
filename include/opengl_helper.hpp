#pragma once

#include <glm/glm.hpp>
#include "geometry.hpp"

GLuint LoadShaders(const char * vertex_file_path,const char * fragment_file_path);
glm::vec3 glmVec3FromVec3f(Vec3f v);
glm::vec3 glmVec3FromVec4f(Vec4f v);
