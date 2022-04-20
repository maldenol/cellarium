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
#include "./render_buffer.hpp"

// External global variables
extern bool gCellRenderingMode;

// Renders cells from buffer
void renderCellBuffer(CellEvolution::CellController &cellController, GLuint shaderProgram,
                      GLuint vao, GLuint vbo) {
  // Using shader program
  glUseProgram(shaderProgram);
  // Binding VAO and VBO
  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  // Getting current count of cells in simulation
  int renderingDataSize = static_cast<int>(cellController.getCellCount());
  // Mapping VBO buffer partly
  CellEvolution::CellController::CellRenderingData *cellRenderingData =
      reinterpret_cast<CellEvolution::CellController::CellRenderingData *>(
          glMapBufferRange(GL_ARRAY_BUFFER, 0, renderingDataSize, GL_MAP_WRITE_BIT));
  // Passing VBO buffer to CellController that fills it with rendering data
  cellController.render(cellRenderingData, gCellRenderingMode);
  // Rendering elements
  glDrawArrays(GL_POINTS, 0, renderingDataSize);
  // Unmapping VBO buffer
  glUnmapBuffer(GL_ARRAY_BUFFER);
}

// Renders photosynthesis energy from buffer
void renderPhotosynthesisEnergyBuffer(GLuint shaderProgram, GLuint vao, GLuint vbo, float offsetX,
                                      float widthScale) {
  // Local constant
  static constexpr int kIndicesCount = 6;

  // Using shader program
  glUseProgram(shaderProgram);
  // Updating shader program uniform variables
  glUniform1f(glGetUniformLocation(shaderProgram, "kOffsetX"), offsetX);
  glUniform1f(glGetUniformLocation(shaderProgram, "kWidthScale"), widthScale);
  // Binding VAO and VBO
  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  // Rendering elements
  glDrawElementsInstanced(GL_TRIANGLES, kIndicesCount, GL_UNSIGNED_INT, 0, 3);
}

// Renders mineral energy from buffer
void renderMineralEnergyBuffer(GLuint shaderProgram, GLuint vao, GLuint vbo) {
  // Local constant
  static constexpr int kIndicesCount = 6;

  // Using shader program
  glUseProgram(shaderProgram);
  // Binding VAO and VBO
  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  // Rendering elements
  glDrawElements(GL_TRIANGLES, kIndicesCount, GL_UNSIGNED_INT, 0);
}
