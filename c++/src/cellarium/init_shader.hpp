// Copyright 2021 Malovanyi Denys Olehovych (maldenol)
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#ifndef INIT_SHADER_HPP
#define INIT_SHADER_HPP

// OpenGL
#include <glad/glad.h>

// Creates, compiles and links cell shader program
GLuint initCellShaderProgram();

// Creates, compiles and links photosynthesis energy shader program
GLuint initPhotosynthesisEnergyShaderProgram();

// Creates, compiles and links mineral energy shader program
GLuint initMineralEnergyShaderProgram();

#endif
