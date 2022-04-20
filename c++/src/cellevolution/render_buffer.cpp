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
void renderCellBuffer(CellEvolution::CellController &cellController, const GLuint cellShaderProgram,
                      GLuint cellVAO) {
  // Getting current count of cells in simulation
  int renderingDataSize = static_cast<int>(cellController.getCellCount());
  // Binding cell VAO
  glBindVertexArray(cellVAO);
  // Mapping VBO buffer partly
  CellEvolution::CellController::RenderingData *renderingData =
      reinterpret_cast<CellEvolution::CellController::RenderingData *>(
          glMapBufferRange(GL_ARRAY_BUFFER, 0, renderingDataSize, GL_MAP_WRITE_BIT));
  // Passing VBO buffer to CellController that fills it with rendering data
  cellController.render(renderingData, gCellRenderingMode);
  // Using cell shader program
  glUseProgram(cellShaderProgram);
  // Rendering cells
  glDrawArrays(GL_POINTS, 0, renderingDataSize);
  // Unmapping VBO buffer
  glUnmapBuffer(GL_ARRAY_BUFFER);
}
