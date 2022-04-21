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

// STD
#include <string_view>

// Qt
#include <QGuiApplication>

// OpenGL
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// "extra" internal library
#include "./extra/extra.hpp"

// Headers of other compile unit
#include "./init_shader.hpp"
#include "./init_buffer.hpp"
#include "./render_buffer.hpp"
#include "./controls.hpp"

// CellController
#include "./cell_controller.hpp"

// Global constants
static constexpr int              kInitWindowWidth                  = 1920;
static constexpr int              kInitWindowHeight                 = 1080;
static constexpr std::string_view kWindowTitle                      = "cellevolution";
static constexpr int              kOpenGLVersionMajor               = 4;
static constexpr int              kOpenGLVersionMinor               = 6;
static constexpr float            kCellSize                         = 8.0f;
static constexpr unsigned int     kRandomSeed                       = 1234567890;
static constexpr float            kMaxPhotosynthesisDepthMultiplier = 0.7f;
static constexpr float            kMaxMineralHeightMultiplier       = 0.7f;

// Main function
int main(int argc, char *argv[]) {
  // Initializing GLFW and getting configured window with OpenGL context
  GLFWwindow *window =
      extra::createWindow(kInitWindowWidth, kInitWindowHeight, std::string{kWindowTitle},
                          kOpenGLVersionMajor, kOpenGLVersionMinor);

  // Capturing OpenGL context
  glfwMakeContextCurrent(window);

  // Setting OpenGL viewport
  glViewport(0, 0, kInitWindowWidth, kInitWindowHeight);
  // Setting window size callback function
  glfwSetWindowSizeCallback(window, windowSizeCallback);
  // Enabling gl_PointSize
  glEnable(GL_PROGRAM_POINT_SIZE);
  // Setting OpenGL clear color
  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
  // Enable blending for environment rendering
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_BLEND);

  // Creating, compiling and linking cell and environment shader programs
  const GLuint cellShaderProgram                 = initCellShaderProgram();
  const GLuint photosynthesisEnergyShaderProgram = initPhotosynthesisEnergyShaderProgram();
  const GLuint mineralEnergyShaderProgram        = initMineralEnergyShaderProgram();

  // Initializing simulation parameters
  CellEvolution::CellController::Params cellControllerParams{};
  cellControllerParams.mersenneTwisterEngine = std::mt19937{kRandomSeed};
  cellControllerParams.randomSeed            = kRandomSeed;
  cellControllerParams.width                 = kInitWindowWidth;
  cellControllerParams.height                = kInitWindowHeight;
  cellControllerParams.cellSize              = kCellSize;
  cellControllerParams.columns = static_cast<int>(static_cast<float>(cellControllerParams.width) /
                                                  cellControllerParams.cellSize);
  cellControllerParams.rows    = static_cast<int>(static_cast<float>(cellControllerParams.height) /
                                               cellControllerParams.cellSize);
  cellControllerParams.maxPhotosynthesisDepth = static_cast<int>(
      static_cast<float>(cellControllerParams.rows) * kMaxPhotosynthesisDepthMultiplier);
  cellControllerParams.maxMineralHeight =
      static_cast<int>(static_cast<float>(cellControllerParams.rows) * kMaxMineralHeightMultiplier);
  // Initializing simulation itself
  CellEvolution::CellController cellController{cellControllerParams};

  // Setting values of cellShaderProgram uniform variables
  glUseProgram(cellShaderProgram);
  glUniform1i(glGetUniformLocation(cellShaderProgram, "kColumns"), cellControllerParams.columns);
  glUniform1i(glGetUniformLocation(cellShaderProgram, "kRows"), cellControllerParams.rows);
  glUniform1f(glGetUniformLocation(cellShaderProgram, "kPointSizeInViewport"),
              cellControllerParams.cellSize);
  glUniform1f(
      glGetUniformLocation(cellShaderProgram, "kPointSizeInClipSpace"),
      cellControllerParams.cellSize /
          static_cast<float>(std::min(cellControllerParams.width, cellControllerParams.height)));
  glUseProgram(0);

  // Initializing and configuring OpenGL Vertex Array and Buffer Objects for cells and environment
  GLuint cellVAO{}, cellVBO{};
  initCellBuffers(cellControllerParams.rows * cellControllerParams.columns, cellVAO, cellVBO);
  GLuint photosynthesisEnergyVAO{}, photosynthesisEnergyVBO{};
  initPhotosynthesisEnergyBuffers(
      static_cast<float>(1 - 2 * cellControllerParams.maxPhotosynthesisDepth /
                                 cellControllerParams.rows),
      photosynthesisEnergyVAO, photosynthesisEnergyVBO);
  GLuint mineralEnergyVAO{}, mineralEnergyVBO{};
  initMineralEnergyBuffers(static_cast<float>(-1 + 2 * cellControllerParams.maxMineralHeight /
                                                       cellControllerParams.rows),
                           mineralEnergyVAO, mineralEnergyVBO);

  // Initializing controls struct
  Controls controls{0, 1, true, true, false, true, true};

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

    // If simulation is not paused
    if (!controls.enablePause) {
      // Computing next simulation tick
      cellController.act();
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

      // Swapping front and back buffers
      glfwSwapBuffers(window);
    }

    // Updating ticks passed value
    ++ticksPassed;
  }

  // Terminating window with OpenGL context and GLFW
  extra::terminateWindow(window);

  return 0;
}
