// Copyright 2021 Malovanyi Denys Olehovych (maldenol)
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

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
