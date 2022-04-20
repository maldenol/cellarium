// This file is part of cellevolution.
//
// cellevolution is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// cellevolution is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with cellevolution.  If not, see <https://www.gnu.org/licenses/>.

// Header file
#include "./init_shader.hpp"

// "extra" internal library
#include "./extra/extra.hpp"

// Creates, compiles and links cell shader program
GLuint initCellShaderProgram() {
  // Initializing vector of shader types
  const std::vector<GLenum> shaderTypes{
      GL_VERTEX_SHADER,
      GL_FRAGMENT_SHADER,
  };

  // Initializing vector of shader sources
  const std::vector<std::string> shaderSources{
      std::string{"#version 460 core\n"
                  "\n"
                  "uniform int   kColumns;\n"
                  "uniform int   kRows;\n"
                  "uniform float kPointSizeInViewport;\n"
                  "uniform float kPointSizeInClipSpace;\n"
                  "\n"
                  "layout (location = 0) in int  aIndex;\n"
                  "layout (location = 1) in vec3 aColor;\n"
                  "\n"
                  "out vec4 fColor;\n"
                  "\n"
                  "void main() {\n"
                  "  int c    = aIndex - aIndex / kColumns * kColumns;\n"
                  "  int r    = aIndex / kColumns;\n"
                  "  float x  = 2.0f * c / kColumns - 1.0f;\n"
                  "  float y  = 2.0f * r / kRows - 1.0f;\n"
                  "  x       += kPointSizeInClipSpace / 2.0f;\n"
                  "  y       += kPointSizeInClipSpace;\n"
                  "\n"
                  "  gl_Position  = vec4(x, -y, 0.0f, 1.0f);\n"
                  "  gl_PointSize = kPointSizeInViewport;\n"
                  "  fColor       = vec4(aColor, 1.0f);\n"
                  "}\n"},
      std::string{"#version 460 core\n"
                  "\n"
                  "in vec4 fColor;\n"
                  "\n"
                  "out vec4 FragColor;\n"
                  "\n"
                  "void main() {\n"
                  "  FragColor = fColor;\n"
                  "}\n"},
  };

  // Creating shader program and returning it
  return extra::createShaderProgram(shaderTypes, shaderSources);
}

// Creates, compiles and links photosynthesis energy shader program
GLuint initPhotosynthesisEnergyShaderProgram() {
  // Initializing vector of shader types
  const std::vector<GLenum> shaderTypes{
      GL_VERTEX_SHADER,
      GL_FRAGMENT_SHADER,
  };

  // Initializing vector of shader sources
  const std::vector<std::string> shaderSources{
      std::string{"#version 460 core\n"
                  "\n"
                  "uniform float kOffsetX;\n"
                  "uniform float kWidthScale;\n"
                  "\n"
                  "layout (location = 0) in vec2  aPosXY;\n"
                  "layout (location = 1) in float aColorA;\n"
                  "\n"
                  "out vec4 fColor;\n"
                  "\n"
                  "float kInstanceOffsetX[3] = {-2.0f, 0.0f, 2.0f};\n"
                  "\n"
                  "void main() {\n"
                  "  float x  = aPosXY.x * kWidthScale + kOffsetX;\n"
                  "  float y  = aPosXY.y;\n"
                  "  x       += kInstanceOffsetX[gl_InstanceID];\n"
                  "  gl_Position = vec4(x, y, 0.0f, 1.0f);\n"
                  "  fColor      = vec4(1.0f, 1.0f, 0.0f, aColorA);\n"
                  "}\n"},
      std::string{"#version 460 core\n"
                  "\n"
                  "in vec4 fColor;\n"
                  "\n"
                  "out vec4 FragColor;\n"
                  "\n"
                  "void main() {\n"
                  "  FragColor = fColor;\n"
                  "}\n"},
  };

  // Creating shader program and returning it
  return extra::createShaderProgram(shaderTypes, shaderSources);
}

// Creates, compiles and links mineral energy shader program
GLuint initMineralEnergyShaderProgram() {
  // Initializing vector of shader types
  const std::vector<GLenum> shaderTypes{
      GL_VERTEX_SHADER,
      GL_FRAGMENT_SHADER,
  };

  // Initializing vector of shader sources
  const std::vector<std::string> shaderSources{
      std::string{"#version 460 core\n"
                  "\n"
                  "layout (location = 0) in vec2  aPosXY;\n"
                  "layout (location = 1) in float aColorA;\n"
                  "\n"
                  "out vec4 fColor;\n"
                  "\n"
                  "void main() {\n"
                  "  gl_Position = vec4(aPosXY.xy, 0.0f, 1.0f);\n"
                  "  fColor      = vec4(0.0f, 0.0f, 1.0f, aColorA);\n"
                  "}\n"},
      std::string{"#version 460 core\n"
                  "\n"
                  "in vec4 fColor;\n"
                  "\n"
                  "out vec4 FragColor;\n"
                  "\n"
                  "void main() {\n"
                  "  FragColor = fColor;\n"
                  "}\n"},
  };

  // Creating shader program and returning it
  return extra::createShaderProgram(shaderTypes, shaderSources);
}
