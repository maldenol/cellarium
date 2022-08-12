// Copyright 2021 Malovanyi Denys Olehovych (maldenol)
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

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
  cellarium::CellController::Params cellControllerParams{};

  // Initializing and configuring Controls struct
  Controls controls{};
  controls.cellRenderingMode          = static_cast<int>(cellarium::CellRenderingModes::Diet);
  controls.ticksPerRender             = 1;
  controls.enableRendering            = true;
  controls.enableRenderingEnvironment = true;
  controls.enablePause                = false;
  controls.enableFullscreenMode       = false;
  controls.enableVSync                = true;
  controls.enableGUI                  = false;
  controls.tickRequest                = false;

  // Processing command line arguments updating CellController::Params and Controls
  int error{processCommandLineArguments(argc, argv, std::string{kWindowTitle}, controls,
                                        cellControllerParams)};
  if (error != 0) {
    return error;
  }

  // Initializing simulation itself
  cellarium::CellController cellController{cellControllerParams};

  // Configuring Controls struct
  controls.cellControllerPtr = &cellController;

  // Initializing GLFW and getting configured window with OpenGL context
  extra::initGLFW();
  GLFWwindow *window =
      extra::createWindow(cellControllerParams.width, cellControllerParams.height,
                          std::string{kWindowTitle}, kOpenGLVersionMajor, kOpenGLVersionMinor);
  if (window == nullptr) {
    return 1;
  }

  // Capturing OpenGL context
  glfwMakeContextCurrent(window);

  // Setting window use pointer for access from callback functions
  glfwSetWindowUserPointer(window, &controls);
  // Setting window size callback function
  glfwSetWindowSizeCallback(window, windowSizeCallback);
  // Setting mouse button callback function
  glfwSetMouseButtonCallback(window, mouseButtonCallback);

  // Initializing Dear ImGui context (after GLFW callbacks specified)
  initDearImGui(window);

  // Setting OpenGL viewport
  glViewport(0, 0, cellControllerParams.width, cellControllerParams.height);
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
        renderMineralEnergyBuffer(mineralEnergyShaderProgram, mineralEnergyVAO);
        renderPhotosynthesisEnergyBuffer(photosynthesisEnergyShaderProgram, photosynthesisEnergyVAO,
                                         cellController.getSunPosition(),
                                         cellController.getDaytimeWidth());
      }

      // Rendering cells
      renderCellBuffer(cellShaderProgram, cellVAO, cellVBO, cellController,
                       controls.cellRenderingMode);

      // Processing Dear ImGui windows
      processDearImGui(window, controls);

      // Swapping front and back buffers
      glfwSwapBuffers(window);
    }

    // Updating ticks passed value
    ++ticksPassed;
  }

  // Terminating Dear ImGui context
  terminateDearImGui();

  // Deleting OpenGL objects
  glDeleteBuffers(1, &mineralEnergyVBO);
  glDeleteVertexArrays(1, &mineralEnergyVAO);
  glDeleteBuffers(1, &photosynthesisEnergyVBO);
  glDeleteVertexArrays(1, &photosynthesisEnergyVAO);
  glDeleteBuffers(1, &cellVBO);
  glDeleteVertexArrays(1, &cellVAO);
  glDeleteProgram(mineralEnergyShaderProgram);
  glDeleteProgram(photosynthesisEnergyShaderProgram);
  glDeleteProgram(cellShaderProgram);

  // Terminating window with OpenGL context and GLFW
  extra::terminateWindow(window);
  extra::terminateGLFW();

  return 0;
}
