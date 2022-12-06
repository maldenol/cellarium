// Copyright 2021 Malovanyi Denys Olehovych (maldenol)
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#ifndef EXTRA_SHADER_HPP
#define EXTRA_SHADER_HPP

// STD
#include <string>
#include <vector>

// OpenGL
#include <glad/glad.h>

namespace extra {

// Compiles shader with source code
void compileShader(GLuint shader, const std::string &source);

// Creates shader program compiling and linking given shaders
GLuint createShaderProgram(const std::vector<GLenum> &shaderTypes, const std::vector<std::string> &shaderSources);

}

#endif
