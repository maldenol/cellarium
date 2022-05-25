// Copyright 2021 Malovanyi Denys Olehovych (maldenol)
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#ifndef CONTROLS_HPP
#define CONTROLS_HPP

// OpenGL
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

// CellController
#include "./cell_controller.hpp"

// Struct with controls variables
struct Controls {
  int  cellRenderingMode{};
  int  ticksPerRender{};

  bool enableRendering{};
  bool enableRenderingEnvironment{};
  bool enablePause{};
  bool enableFullscreenMode{};
  bool enableVSync{};
  bool enableGUI{};

  bool tickRequest{};

  cellarium::CellController *cellControllerPtr{};
  cellarium::Cell            selectedCell{};
};

// User input processing function
void processUserInput(GLFWwindow *window, Controls& controls);

// Window size callback function
void windowSizeCallback(GLFWwindow *window, int width, int height);

// Mouse button callback function
void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods);

// Initializes Dear ImGui context
void initDearImGui(GLFWwindow *window);

// Terminates Dear ImGui context
void terminateDearImGui();

// Processes Dear ImGui windows
void processDearImGui(GLFWwindow *window, Controls &controls);

#endif
