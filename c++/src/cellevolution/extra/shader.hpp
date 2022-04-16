#ifndef EXTRA_SHADER_HPP
#define EXTRA_SHADER_HPP

// STD
#include <string>
#include <vector>

// OpenGL
#define GLFW_INCLUDE_NONE
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace extra {

// Compiles shader with source code
void compileShader(GLuint shader, const std::string &source);

// Creates shader program compiling and linking given shaders
GLuint createShaderProgram(const std::vector<GLenum> &shaderTypes, const std::vector<std::string> &shaderSources);

}

#endif
