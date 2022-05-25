// Copyright 2021 Malovanyi Denys Olehovych (maldenol)
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#ifndef RENDER_BUFFER_HPP
#define RENDER_BUFFER_HPP

// OpenGL
#include <glad/glad.h>

// CellController
#include "./cell_controller.hpp"

// Renders cells from buffer
void renderCellBuffer(GLuint shaderProgram, GLuint vao, GLuint vbo, cellarium::CellController &cellController, int cellRenderingMode);

// Renders photosynthesis energy from buffer
void renderPhotosynthesisEnergyBuffer(GLuint shaderProgram, GLuint vao, GLuint vbo, float offsetX, float widthScale);

// Renders mineral energy from buffer
void renderMineralEnergyBuffer(GLuint shaderProgram, GLuint vao, GLuint vbo);

#endif
