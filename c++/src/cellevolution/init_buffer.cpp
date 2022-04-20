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
#include "./init_buffer.hpp"

// CellController
#include "./cell_controller.hpp"

// Initializes and configures OpenGL Vertex Array and Buffer Objects for cells
GLuint initCellBuffers(GLsizeiptr count) {
  // Creating and binding cell VAO
  GLuint cellVAO{};
  glGenVertexArrays(1, &cellVAO);
  glBindVertexArray(cellVAO);

  // Creating, binding and configuring cell VBO
  GLuint cellVBO{};
  glGenBuffers(1, &cellVBO);
  glBindBuffer(GL_ARRAY_BUFFER, cellVBO);
  glBufferData(
      GL_ARRAY_BUFFER,
      count * static_cast<GLsizeiptr>(sizeof(CellEvolution::CellController::RenderingData)),
      nullptr, GL_DYNAMIC_DRAW);
  glVertexAttribIPointer(0, 1, GL_INT, sizeof(CellEvolution::CellController::RenderingData),
                         reinterpret_cast<void *>(0));
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE,
                        sizeof(CellEvolution::CellController::RenderingData),
                        reinterpret_cast<void *>(sizeof(int)));
  glEnableVertexAttribArray(1);

  // Unbinding cell VAO
  glBindVertexArray(0);

  // Returning cell VAO
  return cellVAO;
}
