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

// Struct with controls variables
struct Controls {
  int  cellRenderingMode{};
  int  ticksPerRender{};
  bool enableRendering{};
  bool enableRenderingEnvironment{};
  bool enablePause{};
  bool enableFullscreenMode{};
  bool enableVSync{};
};

// User input processing function
void processUserInput(GLFWwindow *window, Controls& controls);

// Window size callback function
void windowSizeCallback(GLFWwindow *window, int width, int height);

#endif
