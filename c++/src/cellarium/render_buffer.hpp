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

#ifndef RENDER_BUFFER_HPP
#define RENDER_BUFFER_HPP

// OpenGL
#include <glad/glad.h>

// CellController
#include "./cell_controller.hpp"

// Renders cells from buffer
void renderCellBuffer(GLuint shaderProgram, GLuint vao, GLuint vbo, CellEvolution::CellController &cellController, int cellRenderingMode);

// Renders photosynthesis energy from buffer
void renderPhotosynthesisEnergyBuffer(GLuint shaderProgram, GLuint vao, GLuint vbo, float offsetX, float widthScale);

// Renders mineral energy from buffer
void renderMineralEnergyBuffer(GLuint shaderProgram, GLuint vao, GLuint vbo);

#endif
