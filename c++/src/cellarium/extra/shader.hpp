// This file is part of cellarium.
//
// cellarium is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// cellarium is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with cellarium.  If not, see <https://www.gnu.org/licenses/>.

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
