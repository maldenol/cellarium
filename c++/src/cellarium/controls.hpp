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
