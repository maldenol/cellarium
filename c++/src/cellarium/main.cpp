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

// STD
#include <string_view>

// Qt
#include <QCoreApplication>

// OpenGL
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// "extra" internal library
#include "./extra/extra.hpp"

// Headers of other compile unit
#include "./command_line.hpp"
#include "./init_shader.hpp"
#include "./init_buffer.hpp"
#include "./render_buffer.hpp"
#include "./controls.hpp"

// CellController
#include "./cell_controller.hpp"

// Global constants
static constexpr std::string_view kWindowTitle        = "cellarium";
static constexpr int              kOpenGLVersionMajor = 4;
static constexpr int              kOpenGLVersionMinor = 6;

// Main function
int main(int argc, char *argv[]) {
  // Initializing simulation parameters
  CellEvolution::CellController::Params cellControllerParams{};

  // Initializing controls struct
  Controls controls{0, 1, true, true, false, false, true, false, false};

  // Processing command line arguments updating CellController::Params and Controls
  int success{processCommandLineArguments(argc, argv, std::string{kWindowTitle}, controls,
                                          cellControllerParams)};
  if (success != 0) {
    return 0;
  }

  // Initializing simulation itself
  CellEvolution::CellController cellController{cellControllerParams};

  // Initializing GLFW and getting configured window with OpenGL context
  GLFWwindow *window =
      extra::createWindow(cellControllerParams.width, cellControllerParams.height,
                          std::string{kWindowTitle}, kOpenGLVersionMajor, kOpenGLVersionMinor);

  // Capturing OpenGL context
  glfwMakeContextCurrent(window);

  // Initializing Dear ImGui context
  initDearImGui(window);

  // Setting OpenGL viewport
  glViewport(0, 0, cellControllerParams.width, cellControllerParams.height);
  // Setting window size callback function
  glfwSetWindowSizeCallback(window, windowSizeCallback);
  // Setting OpenGL clear color
  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
  // Enable blending for environment rendering
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_BLEND);

  // Creating, compiling and linking cell and environment shader programs
  const GLuint cellShaderProgram                 = initCellShaderProgram();
  const GLuint photosynthesisEnergyShaderProgram = initPhotosynthesisEnergyShaderProgram();
  const GLuint mineralEnergyShaderProgram        = initMineralEnergyShaderProgram();

  // Setting values of cellShaderProgram uniform variables
  glUseProgram(cellShaderProgram);
  glUniform1i(glGetUniformLocation(cellShaderProgram, "kColumns"), cellController.getColumns());
  glUniform1i(glGetUniformLocation(cellShaderProgram, "kRows"), cellController.getRows());
  glUseProgram(0);

  // Initializing and configuring OpenGL Vertex Array and Buffer Objects for cells and environment
  GLuint cellVAO{}, cellVBO{};
  initCellBuffers(cellController.getColumns() * cellController.getRows(), cellVAO, cellVBO);
  GLuint photosynthesisEnergyVAO{}, photosynthesisEnergyVBO{};
  initPhotosynthesisEnergyBuffers(
      static_cast<float>(1 -
                         2 * cellController.getMaxPhotosynthesisDepth() / cellController.getRows()),
      photosynthesisEnergyVAO, photosynthesisEnergyVBO);
  GLuint mineralEnergyVAO{}, mineralEnergyVBO{};
  initMineralEnergyBuffers(
      static_cast<float>(-1 + 2 * cellController.getMaxMineralHeight() / cellController.getRows()),
      mineralEnergyVAO, mineralEnergyVBO);

  // Initializing ticks passed value
  int ticksPassed{};

  // Render cycle
  while (!glfwWindowShouldClose(window)) {
    // Processing GLFW events
    glfwPollEvents();

    // Processing user input
    processUserInput(window, controls);

    // Checking if current tick should be rendered
    bool renderCurrTick = controls.enableRendering && (ticksPassed % controls.ticksPerRender == 0);

    // If simulation is not paused or a tick is requested
    if (!controls.enablePause || controls.tickRequest) {
      // Computing next simulation tick
      cellController.act();

      // Tick request is satisfied
      controls.tickRequest = false;
    }

    // If current tick should be rendered
    if (renderCurrTick) {
      // Zeroing ticks passed value
      ticksPassed = 0;

      // Clearing color buffer
      glClear(GL_COLOR_BUFFER_BIT);

      // Rendering environment if needed
      if (controls.enableRenderingEnvironment) {
        renderMineralEnergyBuffer(mineralEnergyShaderProgram, mineralEnergyVAO, mineralEnergyVBO);
        renderPhotosynthesisEnergyBuffer(photosynthesisEnergyShaderProgram, photosynthesisEnergyVAO,
                                         photosynthesisEnergyVBO, cellController.getSunPosition(),
                                         cellController.getDaytimeWidth());
      }

      // Rendering cells
      renderCellBuffer(cellShaderProgram, cellVAO, cellVBO, cellController,
                       controls.cellRenderingMode);

      // Drawing Dear ImGui windows
      drawDearImGui(window, controls, cellController);

      // Swapping front and back buffers
      glfwSwapBuffers(window);
    }

    // Updating ticks passed value
    ++ticksPassed;
  }

  // Terminating Dear ImGui context
  terminateDearImGui();

  // Terminating window with OpenGL context and GLFW
  extra::terminateWindow(window);

  return 0;
}
