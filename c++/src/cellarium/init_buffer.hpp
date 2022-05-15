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

#ifndef INIT_BUFFER_HPP
#define INIT_BUFFER_HPP

// OpenGL
#include <glad/glad.h>

// Initializes and configures OpenGL Vertex Array and Buffer Objects for cells
void initCellBuffers(int maxCount, GLuint &vao, GLuint &vbo);

// Initializes and configures OpenGL Vertex Array and Buffer Objects for photosynthesis energy
void initPhotosynthesisEnergyBuffers(float minHeightInClipSpace, GLuint &vao, GLuint &vbo);

// Initializes and configures OpenGL Vertex Array and Buffer Objects for mineral energy
void initMineralEnergyBuffers(float maxHeightInClipSpace, GLuint &vao, GLuint &vbo);

#endif
