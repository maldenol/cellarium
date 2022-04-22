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
#include "./controls.hpp"

// STD
#include <algorithm>

// "extra" internal library
#include "./extra/extra.hpp"

// CellController
#include "./cell_controller.hpp"

// Global constants
static constexpr int kMaxTicksPerRender = 1000;

// User input processing function
void processUserInput(GLFWwindow *window, Controls &controls) {
  // Static variables
  static int sPosX{}, sPosY{}, sWidth{}, sHeight{};

  // Variables for key press and release handling
  static bool sPressed{};
  bool        released = true;

  // Switching cell rendering mode
  if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS) {
    released = false;
    if (!sPressed) {
      sPressed = true;

      controls.cellRenderingMode = (controls.cellRenderingMode + 1) %
                                   static_cast<int>(CellEvolution::CellRenderingModes::Size);
    }
  }

  // Decreasing number of ticks per one rendering
  if (glfwGetKey(window, GLFW_KEY_MINUS) == GLFW_PRESS ||
      glfwGetKey(window, GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS) {
    released = false;
    if (!sPressed) {
      sPressed = true;

      controls.ticksPerRender = std::max(controls.ticksPerRender - 1, 1);
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

      controls.ticksPerRender = std::min(controls.ticksPerRender + 1, kMaxTicksPerRender);
    }
  }

  // Toggling rendering environment flag
  if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
    released = false;
    if (!sPressed) {
      sPressed = true;

      controls.enableRenderingEnvironment = !controls.enableRenderingEnvironment;
    }
  }

  // Toggling rendering flag
  if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
    released = false;
    if (!sPressed) {
      sPressed = true;

      controls.enableRendering = !controls.enableRendering;
    }
  }

  // Toggling pause flag
  if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
    released = false;
    if (!sPressed) {
      sPressed = true;

      controls.enablePause = !controls.enablePause;
    }
  }

  // Toggling window fullscreen mode
  if (glfwGetKey(window, GLFW_KEY_F11) == GLFW_PRESS) {
    released = false;
    if (!sPressed) {
      sPressed = true;

      controls.enableFullscreenMode = !controls.enableFullscreenMode;
      if (controls.enableFullscreenMode) {
        extra::enableFullscreenMode(window, sPosX, sPosY, sWidth, sHeight);
      } else {
        extra::disableFullscreenMode(window, sPosX, sPosY, sWidth, sHeight);
      }
    }
  }

  // Disabling window fullscreen mode
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    released = false;
    if (!sPressed) {
      sPressed = true;

      controls.enableFullscreenMode = false;
      if (glfwGetWindowMonitor(window) != nullptr) {
        extra::disableFullscreenMode(window, sPosX, sPosY, sWidth, sHeight);
      }
    }
  }

  // Toggling window V-sync
  if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS) {
    released = false;
    if (!sPressed) {
      sPressed = true;

      controls.enableVSync = !controls.enableVSync;
      if (controls.enableVSync) {
        glfwSwapInterval(1);
      } else {
        glfwSwapInterval(0);
      }
    }
  }

  // Cheking for key released
  if (released) {
    sPressed = false;
  }

  // Checking initial controls values
  static bool sFirstCall = true;
  if (sFirstCall) {
    sFirstCall = false;

    // Window fullscreen mode
    if (controls.enableFullscreenMode) {
      extra::enableFullscreenMode(window, sPosX, sPosY, sWidth, sHeight);
    }

    // Window V-sync
    if (controls.enableVSync) {
      glfwSwapInterval(1);
    } else {
      glfwSwapInterval(0);
    }
  }
}

// Window size callback function
void windowSizeCallback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
}
