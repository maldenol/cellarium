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

#include <algorithm>
#include <cmath>
#include <iostream>

#include <QGuiApplication>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <cellevolution/cell_controller.hpp>

// Global constants
static constexpr int kInitWindowWidth    = 800;
static constexpr int kInitWindowHeight   = 600;
static constexpr int kOpenGLVersionMajor = 4;
static constexpr int kOpenGLVersionMinor = 6;

// Global variables
int  gCellRenderingMode{0};
int  gTicksPerRender{1};
bool gEnableRendering{true};
bool gEnableRenderingEnvironment{true};
bool gEnablePause{false};

// User input processing function
void processUserInput(GLFWwindow *window);

int main(int argc, char *argv[]) {
  // Initializing GLFW
  if (glfwInit() == GL_FALSE) {
    std::cout << "error: failed to initialize GLFW" << std::endl;

    return -1;
  }
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, kOpenGLVersionMajor);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, kOpenGLVersionMinor);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // Creating GLFW window
  GLFWwindow *window =
      glfwCreateWindow(kInitWindowWidth, kInitWindowHeight, "cellevolution", nullptr, nullptr);
  if (window == nullptr) {
    std::cout << "error: failed to create GLFW window" << std::endl;

    glfwTerminate();
    return -1;
  }

  // Capturing OpenGL context
  glfwMakeContextCurrent(window);

  // Loading OpenGL functions with GLAD
  if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
    std::cout << "error: failed to load OpenGL functions using GLAD" << std::endl;

    glfwMakeContextCurrent(nullptr);
    glfwTerminate();
    return -1;
  }

  // Creating and configuring shader program for rendering cells
  constexpr char *kVertexShaderSource{
      "#version 460 core\n"
      "\n"
      "uniform int kColumns = 1;\n"
      "uniform int kRows = 1;\n"
      "\n"
      "layout (location = 0) in int  aIndex;\n"
      "layout (location = 1) in vec4 aColor;\n"
      "\n"
      "out vec4 fColor;\n"
      "\n"
      "void main() {\n"
      "  int c = aIndex - aIndex / kColumns * kColumns;\n"
      "  int r = aIndex / kColumns;\n"
      "  float x = 2.0f * c / kColumns - 1.0f;\n"
      "  float y = 1.0f - 2.0f * r / kRows;\n"
      "  gl_Position = vec4(x, y, 0.0f, 1.0f);\n"
      "  fColor = aColor;\n"
      "}\n"};
  GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &kVertexShaderSource, nullptr);
  glCompileShader(vertexShader);

  constexpr char *kFragmentShaderSource{
      "#version 460 core\n"
      "\n"
      "in vec4 fColor;\n"
      "\n"
      "out vec4 FragColor;\n"
      "\n"
      "void main() {\n"
      "  FragColor = fColor;\n"
      "}\n"};
  GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &kFragmentShaderSource, nullptr);
  glCompileShader(fragmentShader);

  GLuint shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);
  glDetachShader(shaderProgram, vertexShader);
  glDetachShader(shaderProgram, fragmentShader);
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);
  glUseProgram(shaderProgram);

  // Initializing simulation parameters and simulation itself
  CellController::Params cellControllerParams{};
  CellController         cellController{cellControllerParams};
  // Setting OpenGL viewport
  glViewport(0, 0, cellControllerParams.width, cellControllerParams.height);
  // Setting values of shaderProgram uniform variables
  glUniform1i(glGetUniformLocation(shaderProgram, "kColumns"), cellControllerParams.columns);
  glUniform1i(glGetUniformLocation(shaderProgram, "kRows"), cellControllerParams.rows);
  // Setting gl_PointSize value
  glPointSize(cellControllerParams.cellSize);
  // Getting CellController::RenderingData
  const CellController::RenderingData *renderingData = cellController.getRenderingData();

  // Initializing and configuring OpenGL Vertex Array and Buffer Objects for rendering simulation
  GLuint vao{};
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
  GLuint vbo{};
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER,
               cellControllerParams.columns * cellControllerParams.rows *
                   sizeof(CellController::RenderingData),
               nullptr, GL_DYNAMIC_DRAW);
  glVertexAttribIPointer(0, 1, GL_INT, sizeof(CellController::RenderingData),
                         reinterpret_cast<void *>(0));
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(CellController::RenderingData),
                        reinterpret_cast<void *>(sizeof(int)));
  glEnableVertexAttribArray(1);

  // Enabling gl_PointSize
  glEnable(GL_PROGRAM_POINT_SIZE);

  // Setting OpenGL clear color
  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

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
      cellController.act(enableRenderingEnvironment);
    }

    // If current tick should be rendered
    if (renderCurrTick) {
      // Zeroing ticks passed value
      ticksPassed = 0;

      // Clearing color buffer
      glClear(GL_COLOR_BUFFER_BIT);

      // Updating rendering data of simulation
      cellController.updateRenderingData(gEnableRenderingEnvironment, gCellRenderingMode);

      // Rendering simulation
      int renderingDataSize = cellController.getRenderingDataSize();
      glBufferSubData(GL_ARRAY_BUFFER, 0,
                      renderingDataSize * static_cast<long>(sizeof(CellController::RenderingData)),
                      renderingData);
      glDrawArrays(GL_POINTS, 0, renderingDataSize);

      // Swapping front and back buffers
      glfwSwapBuffers(window);
    }

    // Updating ticks passed value
    ++ticksPassed;
  }

  glfwTerminate();
  return 0;
}

// User input processing function
void processUserInput(GLFWwindow *window) {
  static bool sPressed{};
  bool        released{true};

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
      sPressed        = true;
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
      sPressed        = true;
      gTicksPerRender = std::min(gTicksPerRender + 1, 1000);
    }
  }

  // Toggling rendering environment flag
  if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS) {
    released = false;
    if (!sPressed) {
      sPressed                    = true;
      gEnableRenderingEnvironment = !gEnableRenderingEnvironment;
    }
  }

  // Toggling rendering flag
  if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
    released = false;
    if (!sPressed) {
      sPressed         = true;
      gEnableRendering = !gEnableRendering;
    }
  }

  // Toggling pause flag
  if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
    released = false;
    if (!sPressed) {
      sPressed     = true;
      gEnablePause = !gEnablePause;
    }
  }

  if (released) {
    sPressed = false;
  }
}
