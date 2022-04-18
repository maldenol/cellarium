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
#include <algorithm>
#include <string>

// Qt
#include <QGuiApplication>

// OpenGL
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// "extra" internal library
#include "extra/extra.hpp"

// CellController
#include "./cell_controller.hpp"

// Global constants
static constexpr int     kInitWindowWidth  = 1920;
static constexpr int     kInitWindowHeight = 1080;
static const std::string kWindowTitle{"cellevolution"};
static constexpr int     kOpenGLVersionMajor               = 4;
static constexpr int     kOpenGLVersionMinor               = 6;
static constexpr int     kMaxTicksPerRender                = 1000;
static constexpr float   kCellSize                         = 8.0f;
static constexpr float   kMaxPhotosynthesisDepthMultiplier = 0.7f;
static constexpr float   kMaxMineralHeightMultiplier       = 0.7f;

// Global variables
int  gCellRenderingMode{0};
int  gTicksPerRender{1};
bool gEnableRendering{true};
bool gEnableRenderingEnvironment{true};
bool gEnablePause{false};

// User input processing function
void processUserInput(GLFWwindow *window);

int main(int argc, char *argv[]) {
  // Initializing GLFW and getting configured window with OpenGL context
  GLFWwindow *window = extra::createWindow(kInitWindowWidth, kInitWindowHeight, kWindowTitle,
                                           kOpenGLVersionMajor, kOpenGLVersionMinor);

  // Capturing OpenGL context
  glfwMakeContextCurrent(window);

  // Creating, compiling and linking shader program
  const std::vector<GLenum> shaderTypes{
      GL_VERTEX_SHADER,
      GL_FRAGMENT_SHADER,
  };
  const std::vector<std::string> shaderSources{
      std::string{"#version 460 core\n"
                  "\n"
                  "uniform int   kColumns                = 1;\n"
                  "uniform int   kRows                   = 1;\n"
                  "uniform float kPointSizeInViewport    = 10.0f;\n"
                  "uniform float kPointSizeInClipSpace   = 0.02f;\n"
                  "\n"
                  "layout (location = 0) in int  aIndex;\n"
                  "layout (location = 1) in vec4 aColor;\n"
                  "\n"
                  "out vec4 fColor;\n"
                  "\n"
                  "void main() {\n"
                  "  int c   = aIndex - aIndex / kColumns * kColumns;\n"
                  "  int r   = aIndex / kColumns;\n"
                  "  float x = 2.0f * c / kColumns - 1.0f;\n"
                  "  float y = 2.0f * r / kRows - 1.0f;\n"
                  "  x += kPointSizeInClipSpace / 2.0f;\n"
                  "  y += kPointSizeInClipSpace;\n"
                  "  gl_Position  = vec4(x, -y, 0.0f, 1.0f);\n"
                  "  gl_PointSize = kPointSizeInViewport;\n"
                  "  fColor       = aColor;\n"
                  "}\n"},
      std::string{"#version 460 core\n"
                  "\n"
                  "in vec4 fColor;\n"
                  "\n"
                  "out vec4 FragColor;\n"
                  "\n"
                  "void main() {\n"
                  "  FragColor = fColor;\n"
                  "}\n"},
  };
  const GLuint shaderProgram{extra::createShaderProgram(shaderTypes, shaderSources)};
  // Using shader program
  glUseProgram(shaderProgram);

  // Initializing simulation parameters and simulation itself
  CellEvolution::CellController::Params cellControllerParams{};
  cellControllerParams.width    = kInitWindowWidth;
  cellControllerParams.height   = kInitWindowHeight;
  cellControllerParams.cellSize = kCellSize;
  cellControllerParams.columns  = static_cast<int>(static_cast<float>(cellControllerParams.width) /
                                                  cellControllerParams.cellSize);
  cellControllerParams.rows     = static_cast<int>(static_cast<float>(cellControllerParams.height) /
                                               cellControllerParams.cellSize);
  cellControllerParams.maxPhotosynthesisDepth = static_cast<int>(
      static_cast<float>(cellControllerParams.rows) * kMaxPhotosynthesisDepthMultiplier);
  cellControllerParams.maxMineralHeight =
      static_cast<int>(static_cast<float>(cellControllerParams.rows) * kMaxMineralHeightMultiplier);
  CellEvolution::CellController cellController{cellControllerParams};
  // Setting OpenGL viewport
  glViewport(0, 0, cellControllerParams.width, cellControllerParams.height);
  // Setting values of shaderProgram uniform variables
  glUniform1i(glGetUniformLocation(shaderProgram, "kColumns"), cellControllerParams.columns);
  glUniform1i(glGetUniformLocation(shaderProgram, "kRows"), cellControllerParams.rows);
  glUniform1f(glGetUniformLocation(shaderProgram, "kPointSizeInViewport"),
              cellControllerParams.cellSize);
  glUniform1f(
      glGetUniformLocation(shaderProgram, "kPointSizeInClipSpace"),
      cellControllerParams.cellSize /
          static_cast<float>(std::min(cellControllerParams.width, cellControllerParams.height)));

  // Initializing and configuring OpenGL Vertex Array and Buffer Objects for rendering simulation
  GLuint vao{};
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
  GLuint vbo{};
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER,
               static_cast<long>(cellControllerParams.columns * cellControllerParams.rows *
                                 sizeof(CellEvolution::CellController::RenderingData)),
               nullptr, GL_DYNAMIC_DRAW);
  glVertexAttribIPointer(0, 1, GL_INT, sizeof(CellEvolution::CellController::RenderingData),
                         reinterpret_cast<void *>(0));
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE,
                        sizeof(CellEvolution::CellController::RenderingData),
                        reinterpret_cast<void *>(sizeof(int)));
  glEnableVertexAttribArray(1);

  // Enabling gl_PointSize
  glEnable(GL_PROGRAM_POINT_SIZE);

  // Setting OpenGL clear color
  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

  // Removing FPS limit
  glfwSwapInterval(0);

  // Initializing ticks passed value
  int ticksPassed{};

  // Render cycle
  while (!glfwWindowShouldClose(window)) {
    // Processing GLFW events
    glfwPollEvents();

    // Processing user input
    processUserInput(window);

    // Checking if current tick should be rendered
    bool renderCurrTick             = gEnableRendering && (ticksPassed % gTicksPerRender == 0);
    bool enableRenderingEnvironment = renderCurrTick && gEnableRenderingEnvironment;

    // If simulation is not paused
    if (!gEnablePause) {
      // Computing next simulation tick
      cellController.act();
    }

    // If current tick should be rendered
    if (renderCurrTick) {
      // Zeroing ticks passed value
      ticksPassed = 0;

      // Clearing color buffer
      glClear(GL_COLOR_BUFFER_BIT);

      // Getting current count of cells in simulation
      int renderingDataSize{cellController.getCellCount()};
      // Mapping VBO buffer partly
      CellEvolution::CellController::RenderingData *renderingData =
          reinterpret_cast<CellEvolution::CellController::RenderingData *>(
              glMapBufferRange(GL_ARRAY_BUFFER, 0, renderingDataSize, GL_MAP_WRITE_BIT));
      // Passing VBO buffer to CellController that fills it with rendering data
      cellController.render(renderingData, gCellRenderingMode);
      // Rendering cells
      glDrawArrays(GL_POINTS, 0, renderingDataSize);
      // Unmapping VBO buffer
      glUnmapBuffer(GL_ARRAY_BUFFER);

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

// User input processing function
void processUserInput(GLFWwindow *window) {
  static bool sPressed{};
  bool        released{true};

  static int sPosX{}, sPosY{}, sWidth{}, sHeight{};

  // Switching cell rendering mode
  if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS) {
    released = false;
    if (!sPressed) {
      sPressed = true;

      ++gCellRenderingMode;
    }
  }

  // Decreasing number of ticks per one rendering
  if (glfwGetKey(window, GLFW_KEY_MINUS) == GLFW_PRESS ||
      glfwGetKey(window, GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS) {
    released = false;
    if (!sPressed) {
      sPressed = true;

      gTicksPerRender = std::max(gTicksPerRender - 1, 1);
    }
  }

  // Increasing number of ticks per one rendering
  if ((glfwGetKey(window, GLFW_KEY_EQUAL) == GLFW_PRESS &&
       (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ||
        glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS)) ||
      glfwGetKey(window, GLFW_KEY_KP_ADD) == GLFW_PRESS) {
    released = false;
    if (!sPressed) {
      sPressed = true;

      gTicksPerRender = std::min(gTicksPerRender + 1, kMaxTicksPerRender);
    }
  }

  // Toggling rendering environment flag
  if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS) {
    released = false;
    if (!sPressed) {
      sPressed = true;

      gEnableRenderingEnvironment = !gEnableRenderingEnvironment;
    }
  }

  // Toggling rendering flag
  if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
    released = false;
    if (!sPressed) {
      sPressed = true;

      gEnableRendering = !gEnableRendering;
    }
  }

  // Toggling pause flag
  if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
    released = false;
    if (!sPressed) {
      sPressed = true;

      gEnablePause = !gEnablePause;
    }
  }

  // Toggling window fullscreen mode
  if (glfwGetKey(window, GLFW_KEY_F11) == GLFW_PRESS) {
    released = false;
    if (!sPressed) {
      sPressed = true;

      if (glfwGetWindowMonitor(window) == nullptr) {
        extra::enableFullscreenMode(window, sPosX, sPosY, sWidth, sHeight);
      } else {
        extra::disableFullscreenMode(window, sPosX, sPosY, sWidth, sHeight);
      }
    }
  }

  // Disabling window fullscreen mode if ESC is pressed
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    released = false;
    if (!sPressed) {
      sPressed = true;

      if (glfwGetWindowMonitor(window) != nullptr) {
        extra::disableFullscreenMode(window, sPosX, sPosY, sWidth, sHeight);
      }
    }
  }

  if (released) {
    sPressed = false;
  }
}
