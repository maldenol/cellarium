// Copyright 2021 Malovanyi Denys Olehovych (maldenol)
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

// Header file
#include "./render_buffer.hpp"

// Renders cells from buffer
void renderCellBuffer(GLuint shaderProgram, GLuint vao, GLuint vbo,
                      cellarium::CellController &cellController, int cellRenderingMode) {
  // Binding VBO
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  // Getting current count of cells in simulation
  int renderingDataSize = static_cast<int>(cellController.getCellCount());
  // Mapping VBO buffer partly
  cellarium::CellController::CellRenderingData *cellRenderingData =
      static_cast<cellarium::CellController::CellRenderingData *>(
          glMapBufferRange(GL_ARRAY_BUFFER, 0, renderingDataSize, GL_MAP_WRITE_BIT));

  // Passing VBO buffer to CellController that fills it with rendering data
  cellController.render(cellRenderingData, cellRenderingMode);

  // Unmapping VBO buffer
  glUnmapBuffer(GL_ARRAY_BUFFER);
  // Unbinding VBO
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Using shader program
  glUseProgram(shaderProgram);
  // Binding VAO
  glBindVertexArray(vao);

  // Rendering elements
  glDrawArrays(GL_POINTS, 0, renderingDataSize);

  // Unbinding VAO
  glBindVertexArray(0);
  // Using shader program
  glUseProgram(0);
}

// Renders photosynthesis energy from buffer
void renderPhotosynthesisEnergyBuffer(GLuint shaderProgram, GLuint vao, float offsetX,
                                      float widthScale) {
  // Local constant
  static constexpr int kIndicesCount = 6;

  // Using shader program
  glUseProgram(shaderProgram);
  // Updating shader program uniform variables
  glUniform1f(glGetUniformLocation(shaderProgram, "kOffsetX"), offsetX);
  glUniform1f(glGetUniformLocation(shaderProgram, "kWidthScale"), widthScale);
  // Binding VAO
  glBindVertexArray(vao);

  // Rendering elements
  glDrawElementsInstanced(GL_TRIANGLES, kIndicesCount, GL_UNSIGNED_INT, 0, 3);

  // Unbinding VAO
  glBindVertexArray(0);
  // Using shader program
  glUseProgram(0);
}

// Renders mineral energy from buffer
void renderMineralEnergyBuffer(GLuint shaderProgram, GLuint vao) {
  // Local constant
  static constexpr int kIndicesCount = 6;

  // Using shader program
  glUseProgram(shaderProgram);
  // Binding VAO
  glBindVertexArray(vao);

  // Rendering elements
  glDrawElements(GL_TRIANGLES, kIndicesCount, GL_UNSIGNED_INT, 0);

  // Unbinding VAO
  glBindVertexArray(0);
  // Using shader program
  glUseProgram(0);
}
